#ifndef CLIENT_FUNCTIONS_H
    #define CLIENT_FUNCTIONS_H

    #include "packets.h"

    // declare functions
    int createDataPacket(struct DATA_pkt dataPkt, char* data);
    void initializeDataPacket(struct DATA_pkt* dataPkt, int packetNum, int packetLen, char** payload, int endPacketID);
    void readResponsePacket(char* buff);
    void handleRejectPacket(char* buff, struct REJECT_pkt* rejectPkt);
    void bufferToStruct(char* buffer, void* x, int* index, int numBytes);
    void structToBuffer(char* buffer, void* x, int* index, int numBytes);

#endif