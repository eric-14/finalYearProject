#ifndef CAN_COMMUNICATION_H
#define CAN_COMMUNICATION_H


#include <Arduino.h>

// #include <SPI.h>
class CAN_PROTOCOL{

    public: 
        CAN_PROTOCOL(int canINT,int canCS);
        void messageDetails(unsigned long ID, char msg[]);        
        void canMain();
        //This should all be private 
        //I did not have time to implement 
        uint16_t volt, curr , remCap;
        uint16_t fullCap, rSOC , nCycles;
        uint16_t NT1, NT2 , NT3;
        uint16_t NT4, NT5 , NT6;

    private:
        long unsigned int rxId;
        unsigned char len = 0;
        unsigned char rxBuf[8];
        
        char msgString[128];                        // Array to store serial string
        void sendRemoteMessages();
        unsigned int messageLen = 8;
        int CAN0_INT = 7;                          // Set INT to pin 2
        int canCS = 10;
        MCP_CAN CAN0; 
     
};

extern CAN_PROTOCOL can;
#endif