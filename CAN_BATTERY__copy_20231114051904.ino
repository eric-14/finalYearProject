// CAN Send Example
//


#define CRC_16_POLYNOMIAL 0xA001


#include <mcp_can.h>
#include <SPI.h>
#define CAN0_INT 7 // Interrupt pin when low CAN message has been received


// typedef unsigned char uint8_t;
// typedef unsigned short uint16_t;

int messageLen = 8;


uint16_t CheckCRC16(uint8_t* pchMsg, uint8_t wDataLen);

uint8_t auchCRCHi[] = { 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
    
    };


uint8_t auchCRCLo[] = {
        0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
        0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
        0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
        0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
        0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
        0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
        0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
        0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
        0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
        0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
        0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
        0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
        0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
        0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
        0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
        0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
        0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
        0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
        0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
        0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
        0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
        0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
        0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
        0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
        0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
};


long unsigned int rxID; // battery should receive id 11 
unsigned char len = 0; 
unsigned char rxBuf[8]; 




unsigned int rxBatteryID = 0x11; 


MCP_CAN CAN0(10);     // Set CS to pin 10

void setup()
{
  Serial.begin(115200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");
  /*
    Interrupt pin to indicate can received
  */
  pinMode(CAN0_INT, INPUT);
  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

uint8_t data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void loop()
{
  if(!digitalRead(CAN0_INT)){
     Serial.println(" RECEIVED CAN MESSAGE ");
  }
  CAN0.readMsgBuf(&rxID, &len , rxBuf); 


  bool RTR = rxID | 0x40000000; 

 

  Serial.print(" Verification of REMOTE CAN message ");
  Serial.print(RTR);
  Serial.print(" ");
  Serial.println(rxID, HEX);

  if(!(RTR))
  {

    Serial.print(" DATA FRAME SENT ");
    Serial.println (rxID,HEX);
  } else {
    Serial.print(" REMOTE FRAME SENT    We definitely can work with this");
    Serial.println(rxID);

    if(rxID == 17)
    {
      /**
        * confirmed the standard ID is similar to that of the battery 
        * check if the CAN packet is a renote frame 
      */
      

          /*
            * Confirmed received a remote packet 


          */

          Serial.println(" ^^^^ Received a REMOTE PACKET ^^^^^^ "); 
          Serial.println(); 
          Serial.println(" Responding with battery data ");
          /*
            ID = 0X100, STANDARD CAN FRAME, DATA LENGTH = 8, DATA 
            voltage 80,000mV
            current 10,0000mA

            remaining caapcity 2000mAh 


          */
          unsigned long int voltage = 80000; 
          unsigned long int current = 100000;
          unsigned long int remCap = 2000;


          //sending voltage data
          data[0] = highByte(voltage);
          data[1] = lowByte(voltage); 
          //sending current 
          data[2] = highByte(current);
          data[3] = lowByte(current);

          //sending remaining caapacitty 
          data[4] = highByte(remCap);
          data[5] = lowByte(remCap);
          data[6] = highByte(CheckCRC16(&data[0], 6));
          data[7] = lowByte(CheckCRC16(&data[0], messageLen-2));
          //sent data on voltage, current and remaining capacity 
          Serial.println(" Sending Message on voltage , current and remaining capacity "); 
          CAN0.sendMsgBuf(0x100, 0, messageLen,&data[0]);
          delay(300);
          

          unsigned int fullCapacity = 100000; 
          unsigned int NoOfDischargeCycles = 4;
            // 
            /*
              relative rate of discharge- 
              is a measure of the ramining caapcity of a battery's SOC relative to a refernce state. 
              usually the fully charged state 

              How much energy is left in the battery compared to its maximum capacity
              0% - indicates empty 
              100% - indicates full
            */ 
          unsigned int RSOC = 80;                      
          //sending full capacity
          data[0] = highByte(fullCapacity);
          data[1] = lowByte(fullCapacity); 
          //Number of Discharge Cycles 
          data[2] = highByte(NoOfDischargeCycles);
          data[3] = lowByte(NoOfDischargeCycles);

          //sending remaining caapacitty 
          data[4] = highByte(RSOC);
          data[5] = lowByte(RSOC);
          data[6] = highByte(CheckCRC16(&data[0], 6));
          data[7] = lowByte(CheckCRC16(&data[0], messageLen-2));

          Serial.println(" Sending full capacity, number of cycles and RSOC [Resulting State of Charge ] ");
          CAN0.sendMsgBuf(0x101, 0, messageLen,&data[0]);

          delay(300);

          /**
            * at 0 = 2731 + 0 x 10 
            at 50 = 2731 + 50 x 10 = 3231 
            at 60 = 2731 + 60 x 10  = 3331 
          
          */

          unsigned int NTC1 = 3231 ; 
          unsigned int NTC2 = 2631; 
          unsigned int NTC3 = 3331; 


          data[0] = highByte(NTC1);
          data[1] = lowByte(NTC1); 
          //Number of Discharge Cycles 
          data[2] = highByte(NTC2);
          data[3] = lowByte(NTC2);

          //sending remaining caapacitty 
          data[4] = highByte(NTC3);
          data[5] = lowByte(NTC3);
          data[6] = highByte(CheckCRC16(&data[0], 6));
          data[7] = lowByte(CheckCRC16(&data[0], messageLen-2));

          Serial.println(" Sending VALUE FOR TEMPERATURE PROBES NTC1, NTC2 NTC3  ");


          CAN0.sendMsgBuf(0x105, 0, messageLen,&data[0]);

          delay(300);

          unsigned int NTC4 = 3231 ; 
          unsigned int NTC5 = 2631; 
          unsigned int NTC6 = 3331; 


          data[0] = highByte(NTC4);
          data[1] = lowByte(NTC4); 
          //Number of Discharge Cycles 
          data[2] = highByte(NTC5);
          data[3] = lowByte(NTC5);

          //sending remaining caapacitty 
          data[4] = highByte(NTC6);
          data[5] = lowByte(NTC6);
          data[6] = highByte(CheckCRC16(&data[0], 6));
          data[7] = lowByte(CheckCRC16(&data[0], messageLen-2));

          Serial.println(" Sending VALUE FOR TEMPERATURE PROBES NTC4, NTC5 NTC6  ");
          CAN0.sendMsgBuf(0x105, 0, messageLen,&data[0]);

          delay(300);


       
    }
  }
  

  
  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  // byte sndStat = CAN0.sendMsgBuf(0x100, 0, 8, data);
  // Serial.println("Battery has sent voltage, current and remaining battery capacity.  ");
  // delay(200);

  // if(sndStat == CAN_OK){
  //   Serial.println("Message Sent Successfully!");
  // } else {
  //   Serial.println("Error Sending Message...");
  // }
  // delay(100);   // send data per 100ms
}
void FNBattery()
{
  /*
  from battery data sheet 
  baud rate 500khz id 11 
  host send instructions remote frame instruction 
  board responds 
  with data frame 
  
  */

}







uint16_t CheckCRC16(uint8_t* pchMsg, uint8_t wDataLen) {
    uint8_t i;
    uint16_t wCRC = 0xFFFF;
    

    while (wDataLen--) {
        uint8_t chChar = *pchMsg++;
        wCRC ^= static_cast<uint16_t>(chChar);
        for (i = 0; i < 8; i++) {
            if (wCRC & 0x0001) {
                wCRC = static_cast<uint16_t>((wCRC >> 1) ^ CRC_16_POLYNOMIAL);
            } else {
                wCRC >>= 1;
            }
        }
    }

    return wCRC;
}
