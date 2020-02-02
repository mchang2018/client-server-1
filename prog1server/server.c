#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "packets.h"
#include "server_functions.h"


int main() {
    //declare variables
    int sockfd; /*int newsockfd = 0*/;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    struct DATA_pkt dataPacket;
    struct ACK_pkt ackPacket;
    struct REJECT_pkt rejectPacket;
    int n;
    int packetNum = 0;

    // create server socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        printf("Error creating server socket.\n");
        exit(1);
    }  

    // initialize socket structure
    bzero((char*) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7572);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // bind serverAddr to socket
    bind(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));

    // initialize structs for the sender
    struct sockaddr client;
    socklen_t clientSize = sizeof(client);
    memset(&client, 0, sizeof(client));

    while(1) {
        // reset the buffer
        memset(&buffer, 0, sizeof(buffer));

        // receive data packet from client
        recvfrom(sockfd, buffer, sizeof(buffer), 0, &client, &clientSize);

        // check if packet is correct and send appropriate ACK or REJECT packet

        // read the data packet
        int actualPayloadLength;
        n = readDataPacket(buffer, &dataPacket, &actualPayloadLength);

        // this variable keeps track of which packet number is being received
        packetNum++;

        int pktLen;

        // Check for error case 3 - no end of packet id 
        if (n == 0) {
            // Check for error case 1 - received packet out of sequence
            if (packetNum < dataPacket.segmentNum) {
                printf("Received packet Out of Sequence\n");
                // initialize REJECT packet with OUT_OF_SEQUENCE reject code
                rejectPacket.startID = START_OF_PACKET_ID;
                rejectPacket.clientID = 3;
                rejectPacket.packetType = REJECT_PACKET;
                rejectPacket.rejectCode = OUT_OF_SEQUENCE;
                rejectPacket.receivedSegmentNum = dataPacket.segmentNum;
                rejectPacket.endID = END_OF_PACKET_ID;

                // build REJECT packet
                pktLen = createREJECTPacket(rejectPacket, buffer);
            }
            // Check for error case 2 - length mismatch
            else if (dataPacket.dataLength != actualPayloadLength) {
                printf("Payload length does not match expected length\n");
                // initialize REJECT packet with LENGTH_MISMATCH reject code
                rejectPacket.startID = START_OF_PACKET_ID;
                rejectPacket.clientID = 3;
                rejectPacket.packetType = REJECT_PACKET;
                rejectPacket.rejectCode = LENGTH_MISMATCH;
                rejectPacket.receivedSegmentNum = dataPacket.segmentNum;
                rejectPacket.endID = END_OF_PACKET_ID;

                // build REJECT packet
                pktLen = createREJECTPacket(rejectPacket, buffer);
            }
            // Check for error case 4 - duplicate packet
            else if (dataPacket.segmentNum < packetNum) {
                printf("Received packet is a duplicate packet\n");
                // initialize REJECT packet with DUPLICATE_PACKET reject code
                rejectPacket.startID = START_OF_PACKET_ID;
                rejectPacket.clientID = 3;
                rejectPacket.packetType = REJECT_PACKET;
                rejectPacket.rejectCode = DUPLICATE_PACKET;
                rejectPacket.receivedSegmentNum = dataPacket.segmentNum;
                rejectPacket.endID = END_OF_PACKET_ID;

                // build REJECT packet
                pktLen = createREJECTPacket(rejectPacket, buffer);
            }
            // No error - send ACK packet
            else {
                printf("Sending ACK packet\n");
                ackPacket.startID = START_OF_PACKET_ID;
                ackPacket.clientID = 2;
                ackPacket.packetType = ACK_PACKET;
                ackPacket.receivedSegmentNum = dataPacket.segmentNum;
                ackPacket.endID = END_OF_PACKET_ID;

                // build ACK packet
                pktLen = createACKPacket(ackPacket, buffer);
            }
        }
        else {
            printf("End of Packet Identifier missing\n");
            // initalize REJECT packet with END_OF_PACKET_MISSING reject code
            rejectPacket.startID = START_OF_PACKET_ID;
            rejectPacket.clientID = 3;
            rejectPacket.packetType = REJECT_PACKET;
            rejectPacket.rejectCode = END_OF_PACKET_MISSING;
            rejectPacket.receivedSegmentNum = dataPacket.segmentNum;
            rejectPacket.endID = END_OF_PACKET_ID;

            // build REJECT packet
            pktLen = createREJECTPacket(rejectPacket, buffer);
        }

        // send appropriate packet to client
        if (sendto(sockfd, buffer, pktLen, 0, &client, clientSize) < 0) {
            printf("ERROR! PACKET NOT SENT.\n\n");
        }
        else {
            printf("RESPONSE PACKET SUCCESSFULLY SENT TO CLIENT!\n\n");
        }
    }

    return 0;
}
