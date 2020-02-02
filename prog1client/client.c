#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#include "packets.h"
#include "client_functions.h"


int main() {
    // declare variables
    int sockfd, pktLen; 
    int pkt = 1;
    struct sockaddr_in serverAddr;
    struct DATA_pkt dataPacket;
    char buffer[1024];
    char* correctPayload = "test"; 

    // create client socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        printf("Error creating client socket.\n");
        exit(1);
    }

    // initialize serverAddr information
    bzero((char*) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7572);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // use S0_RCVTIME0 to set a timeout value for input operations to complete
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) {
        perror("setsockopt failed\n");
    }

    /* TO SEND 5 CORRECT PACKETS, UNCOMMENT LINES 49-89 AND COMMENT OUT LINES 91-159 AND REPEAT COMPILING INSTRUCTIONS
       DO THE VICE VERSA TO SEND 1 CORRECT PACKET AND 4 INCORRECT PACKETS (program is set to send 1 correct, 4 wrong first) */
    
    // printf("Sending 5 correct packets\n\n");

    // while (pkt <= 5) {
    //     //initalize correct data packet
    //     initializeDataPacket(&dataPacket, pkt, 4, &correctPayload, END_OF_PACKET_ID);

    //     // create the data packet to be sent
    //     pktLen = createDataPacket(dataPacket, buffer);

    //     // send the data packet to server and increment pkt count
    //     sendto(sockfd, buffer, pktLen, 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    //     printf("Sending data packet %d\n", pkt);
        
    //     // Read the response packet and re-transmit if necessary
    //     struct sockaddr from;
    //     socklen_t fromSize = sizeof(from);
    //     memset(&from, 0, sizeof(from));
    //     bind(sockfd, &from, fromSize);
    //     char buffer2[1024];
        
    //     int counter = 0;
    //     while (counter < 3) {
    //         // get response packet and then read it
    //         if (recvfrom(sockfd, buffer2, sizeof(buffer2), 0, (struct sockaddr* )&from, &fromSize) >= 0) {
    //             // determine whether response packet is ACK or REJECT and handle response accordingly
    //             readResponsePacket(buffer2);
    //             break;
    //         }
    //         else {
    //             // no response from server - increment counter and then re-transmit
    //             counter++;
    //             sendto(sockfd, buffer, pktLen, 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    //         }
    //     }
    //     // if client has tried sending same packet 3 times already and server still doesn't respond, print error
    //     if (counter == 3) {
    //         printf("Server does not respond\n");
    //     }
    //     pkt++;
    //     printf("\n");
    // }

    printf("\n");
    printf("Sending 1 correct packet and 4 error packets\n\n");

    pkt = 1;
    while (pkt <= 5) {
        if(pkt == 1) {
            // send correct packet
            initializeDataPacket(&dataPacket, 1, 4, &correctPayload, END_OF_PACKET_ID);
            pktLen = createDataPacket(dataPacket, buffer);
            sendto(sockfd, buffer, pktLen, 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
            printf("Sending data packet 1\n");
        }
        else if (pkt == 2) {
            // Error Case 1 - Send out of sequence packet
            initializeDataPacket(&dataPacket, 11, 4, &correctPayload, END_OF_PACKET_ID);
            pktLen = createDataPacket(dataPacket, buffer);
            sendto(sockfd, buffer, pktLen, 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
            printf("Sending data packet 2\n");
        }
        else if (pkt == 3) {
            // Error Case 2 - Lengths don't match
            initializeDataPacket(&dataPacket, 3, 11, &correctPayload, END_OF_PACKET_ID);
            pktLen = createDataPacket(dataPacket, buffer);
            sendto(sockfd, buffer, pktLen, 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
            printf("Sending data packet 3\n");
        }
        else if (pkt == 4) {
            // Error Case 3 - Missing end of packet identifier
            initializeDataPacket(&dataPacket, 4, 4, &correctPayload, 0);
            pktLen = createDataPacket(dataPacket, buffer);
            sendto(sockfd, buffer, pktLen, 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
            printf("Sending data packet 4\n");
        }
        else if (pkt == 5) {
            // Error Case 4 - Duplicate Packet
            initializeDataPacket(&dataPacket, 1, 4, &correctPayload, END_OF_PACKET_ID);
            pktLen = createDataPacket(dataPacket, buffer);
            sendto(sockfd, buffer, pktLen, 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
            printf("Sending data packet 5\n");
        }

        // Read the response packet and re-transmit if necessary
        struct sockaddr from;
        socklen_t fromSize = sizeof(from);
        memset(&from, 0, sizeof(from));
        bind(sockfd, &from, fromSize);
        char buffer2[1024];
        
        int counter = 0;
        while (counter < 3) {
            // get response packet and then read it
            if (recvfrom(sockfd, buffer2, sizeof(buffer2), 0, (struct sockaddr* )&from, &fromSize) >= 0) {
                // determine whether response packet is ACK or REJECT and handle response accordingly
                readResponsePacket(buffer2);
                break;
            }
            else {
                // no response from server - increment counter and then re-transmit
                counter++;
                sendto(sockfd, buffer, pktLen, 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
            }
        }
        // if client has tried sending same packet 3 times already and server still doesn't respond, print error
        if (counter == 3) {
            printf("Server does not respond\n");
        }
        pkt++;
        printf("\n");
    }
    return 0;
}