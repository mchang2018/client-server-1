#include <stdio.h>
#include <string.h>

#include "packets.h"
#include "server_functions.h"


// This function copies data from buffer to struct using memcpy
void bufferToStruct(char* buffer, void* x, int* index, int numBytes) {
    memcpy(x, &(buffer[*index]), numBytes);
    *index += numBytes;
}


// This function copies data from struct to buffer using memcpy
void structToBuffer(char* buffer, void* x, int* index, int numBytes) {
    memcpy(&(buffer[*index]), x, numBytes);
    *index += numBytes;
}


// function to create ACK packet
int createACKPacket(struct ACK_pkt ACKpacket, char* ackBuffer) {
    int ackLen = 0;

    // put start of packet id into ACK buffer (2 bytes) using memcpy
    structToBuffer(ackBuffer, &(ACKpacket.startID), &ackLen, 2);

    // put client id into ACK buffer (1 byte)
    structToBuffer(ackBuffer, &(ACKpacket.clientID), &ackLen, 1);

    // put packet type into ACK buffer (2 bytes)
    structToBuffer(ackBuffer, &(ACKpacket.packetType), &ackLen, 2);

    // put received segment number into ACK buffer (1 byte)
    structToBuffer(ackBuffer, &(ACKpacket.receivedSegmentNum), &ackLen, 1);

    // put end of packet id into ACK buffer (2 bytes)
    structToBuffer(ackBuffer, &(ACKpacket.endID), &ackLen, 2);

    // return the length of ACK packet
    return ackLen;
}


// function to create REJECT packet
int createREJECTPacket(struct REJECT_pkt REJECTpacket, char* rejectBuffer) {
    int rejectLen = 0;

    // put start of packet id into REJECT buffer (2 bytes) 
    structToBuffer(rejectBuffer, &(REJECTpacket.startID), &rejectLen, 2);

    // put client id into REJECT buffer (1 byte) 
    structToBuffer(rejectBuffer, &(REJECTpacket.clientID), &rejectLen, 1);

    // put packet type into REJECT buffer (2 bytes) 
    structToBuffer(rejectBuffer, &(REJECTpacket.packetType), &rejectLen, 2);

    // put reject sub code into REJECT buffer (2 bytes)
    structToBuffer(rejectBuffer, &(REJECTpacket.rejectCode), &rejectLen, 2);

    //  put received segment number into REJECT buffer (1 byte)
    structToBuffer(rejectBuffer, &(REJECTpacket.receivedSegmentNum), &rejectLen, 1);

    // put end of packet id into REJECT buffer (2 bytes)
    structToBuffer(rejectBuffer, &(REJECTpacket.endID), &rejectLen, 2);

    // return length of REJECT packet
    return rejectLen;
}


// this function takes in and reads a data packet
int readDataPacket(char* buffer, struct DATA_pkt* dataPacket, int* actualPayloadLen) {
    // copy contents of the buffer to the data packet structure
    int bufferLen = 0;

    // copy start of packet id to dataPacket
    bufferToStruct(buffer, &(dataPacket->startID), &bufferLen, 2);

    // copy client ID to dataPacket
    bufferToStruct(buffer, &(dataPacket->clientID), &bufferLen, 1);

    // copy client packet type to dataPacket
    bufferToStruct(buffer, &(dataPacket->packetType), &bufferLen, 2);

    // copy the segment number to dataPacket
    bufferToStruct(buffer, &(dataPacket->segmentNum), &bufferLen, 1);

    // copy payload length to dataPacket
    bufferToStruct(buffer, &(dataPacket->dataLength), &bufferLen, 1);

    // copy the payload and end id to dataPacket
    int payloadLen;

    for (payloadLen = 0; payloadLen < 255; payloadLen++) {
        // check if the next 2 bytes are end packet ID. If yes, that means we've reached end of payload
        int checkEndID;
        memcpy(&checkEndID, &(buffer[bufferLen]), 2);

        if (checkEndID == END_OF_PACKET_ID) {
            // reached end of payload - set end of packet ID then return 0
            memcpy(&(dataPacket->endID), &checkEndID, 2);
            *actualPayloadLen = payloadLen;
            bufferLen += 2;
            // printf("%d\n", checkEndID);
            return 0;
        }
        else {
            // not at end of payload - copy another byte of payload data to dataPacket
            memcpy(&(dataPacket->payload[payloadLen]), &(buffer[bufferLen]), 1);
            // printf("%c\n", dataPacket->payload[payloadLen]);
            bufferLen++;
        }
    }
    // if we get here, check again if the next 2 bytes are end packet ID. If not, it's missing
    int checkEnd;
    memcpy(&checkEnd, &(buffer[bufferLen]), 2);

    if (checkEnd == END_OF_PACKET_ID) {
        // reached end of payload - set end of packet ID then return 0
        memcpy(&(dataPacket->endID), &checkEnd, 2);
        *actualPayloadLen = payloadLen;
        bufferLen += 2;
        return 0;
    }
    // end of packet ID missing - return END_OF_PACKET_MISSING flag
    return END_OF_PACKET_MISSING;
}