#ifndef SERVER_FUNCTIONS_H
    #define SERVER_FUNCTIONS_H

    #include "packets.h"

    // declare functions
    int createACKPacket(struct ACK_pkt ACKpacket, char* ackBuffer);
    int createREJECTPacket(struct REJECT_pkt REJECTpacket, char* rejectBuffer);
    int readDataPacket(char* buffer, struct DATA_pkt* dataPacket, int* actualPayloadLen);
    void bufferToStruct(char* buffer, void* x, int* index, int numBytes);
    void structToBuffer(char* buffer, void* x, int* index, int numBytes); 

#endif