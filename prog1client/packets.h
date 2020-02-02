#ifndef PACKETS_H
    #define PACKETS_H

    // start and end of packet IDs
    #define START_OF_PACKET_ID 0xFFFF
    #define END_OF_PACKET_ID 0xFFFF

    // packet type codes
    #define DATA_PACKET 0XFFF1
    #define ACK_PACKET 0XFFF2
    #define REJECT_PACKET 0XFFF3

    // reject sub codes
    #define OUT_OF_SEQUENCE 0XFFF4
    #define LENGTH_MISMATCH 0xFFF5
    #define END_OF_PACKET_MISSING 0xFFF6
    #define DUPLICATE_PACKET 0xFFF7


    // make struct for data packet
    struct DATA_pkt {
        unsigned int startID;
        unsigned int clientID;
        unsigned int packetType;
        unsigned int segmentNum;
        unsigned int dataLength;
        char payload[255];
        unsigned int endID;
    };

    // make struct for ACK packet
    struct ACK_pkt {
        unsigned int startID;
        unsigned int clientID;
        unsigned int packetType;
        unsigned int receivedSegmentNum;
        unsigned int endID;
    };

    // make struct for REJECT packet
    struct REJECT_pkt {
        unsigned int startID;
        unsigned int clientID;
        unsigned int packetType;
        unsigned int rejectCode;
        unsigned int receivedSegmentNum;
        unsigned int endID;
    };

#endif
