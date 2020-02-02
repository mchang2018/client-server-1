#include <stdio.h>
#include <string.h>

#include "packets.h"
#include "client_functions.h"


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


// function to create a data packet
int createDataPacket(struct DATA_pkt dataPkt, char* data) {
    int dataLen = 0;

    // put start of packet id into data buffer (2 bytes) using the memcpy function, then increment dataLen
    structToBuffer(data, &(dataPkt.startID), &dataLen, 2);

    // put client id into data buffer (1 byte), then increment dataLen
    structToBuffer(data, &(dataPkt.clientID), &dataLen, 1);

    // put packet type into data buffer (2 bytes), then increment dataLen
    structToBuffer(data, &(dataPkt.packetType), &dataLen, 2);

    // put segment number into data buffer (1 byte), then increment dataLen
    structToBuffer(data, &(dataPkt.segmentNum), &dataLen, 1);

    // put payload length into data buffer (1 byte), then increment dataLen
    structToBuffer(data, &(dataPkt.dataLength), &dataLen, 1);

    // insert payload into data buffer and increment dataLen by the length of payload
    memcpy(&data[dataLen], &dataPkt.payload, 4);
    dataLen = dataLen + dataPkt.dataLength;

    // put end of packet id into data buffer (2 bytes), then increment dataLen
    structToBuffer(data, &(dataPkt.endID), &dataLen, 2);

    // return the length of data packet
    return dataLen;
}

// this function populates a DATA_pkt structure with correct information
void initializeDataPacket(struct DATA_pkt* dataPkt, int packetNum, int packetLen, char** payloadData, int endPacketID) {
    dataPkt->startID = START_OF_PACKET_ID;
    dataPkt->clientID = 1;
    dataPkt->packetType = DATA_PACKET;
    dataPkt->segmentNum = packetNum;
    dataPkt->dataLength = packetLen;
    memcpy(&(dataPkt->payload), &payloadData, 4);
    dataPkt->endID = endPacketID;
}


// this function determines whether a recieved response packet is ACK or REJECT and handles response accordingly
void readResponsePacket(char* buff) {
    int pktType;
    struct REJECT_pkt rejectPacket;

    // get the packet type of response
    memcpy(&pktType, &(buff[3]), 2);
    // printf("%d\n", pktType);
    // check packet type
    switch(pktType) {
        case ACK_PACKET:
            printf("ACK received\n");
            break;
        case REJECT_PACKET:
            handleRejectPacket(buff, &rejectPacket);
            break;
    }
}


// this function reads the reject packet for correct reject code, then prints it out
void handleRejectPacket(char* buff, struct REJECT_pkt* rejectPkt) {
    int rejectLen = 0;

    // transfer contents of buffer back to a REJECT_pkt structure
    bufferToStruct(buff, &(rejectPkt->startID), &rejectLen, 2);

    bufferToStruct(buff, &(rejectPkt->clientID), &rejectLen, 1);

    bufferToStruct(buff, &(rejectPkt->packetType), &rejectLen, 2);

    bufferToStruct(buff, &(rejectPkt->rejectCode), &rejectLen, 2);

    bufferToStruct(buff, &(rejectPkt->receivedSegmentNum), &rejectLen, 1);

    bufferToStruct(buff, &(rejectPkt->endID), &rejectLen, 2);

    // print out the right reject error msg based on sub code
    if (rejectPkt->rejectCode == OUT_OF_SEQUENCE) {
        printf("REJECT (sub code %d) received - PACKET OUT OF SEQUENCE\n", rejectPkt->rejectCode);
    }
    else if (rejectPkt->rejectCode == LENGTH_MISMATCH) {
        printf("REJECT (sub code %d) received - LENGTH MISMATCH\n", rejectPkt->rejectCode);
    }
    else if (rejectPkt->rejectCode == END_OF_PACKET_MISSING) {
        printf("REJECT (sub code %d) received - END OF PACKET ID MISSING\n", rejectPkt->rejectCode);
    }
    else if (rejectPkt->rejectCode == DUPLICATE_PACKET) {
        printf("REJECT (sub code %d) received - DUPLICATE PACKET\n", rejectPkt->rejectCode);
    }
}