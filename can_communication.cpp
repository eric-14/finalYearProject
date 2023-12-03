#include "CAN_COMMUNICATION.h"

void CAN_PROTOCOL::canMain()
{
   /*
    *
     to receive battery information start by sending a remote CAN message
  
  */
  sendRemoteMessages();

  if(digitalRead(CAN0_INT)){
      //Serial.println("CAN INT PIN HIGH 1"); 
  }else if(!digitalRead(CAN0_INT)){
      Serial.println(" CAN LINE LOW  READY TO RECEIVE DATA ");
  }
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    Serial.println("      Receiving CAN messages      ");
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    Serial.println(" printing msg string ");

    if((rxId & 0x80000000) == 0x80000000){     // Determine if ID is standard (11 bits) or extended (29 bits)
      Serial.println("Extended ID:   DLC "); 
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    }else{
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
        messageDetails(rxId, rxBuf);
       Serial.println("Standard ID:   DLC ");
    }
    Serial.print( msgString);
  
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print( msgString);
      }
    }
        
    Serial.println();
  }
}
void CAN_PROTOCOL::sendRemoteMessages(){

    uint8_t data[8] = { 0x00, 0x01 , 0x02 , 0x03 , 0x04, 0x05 , 0x06, 0x07 };
    Serial.println(" Sending CAN Remote Message to Battery ");
    unsigned long canID = 0x11 | 0x40000000; 
    

    
    CAN0.sendMsgBuf(canID, 0, 8,   &data[0]);
    Serial.print(" CAN Message sent "); 
    Serial.println(canID);
    delay(300);

}
 void CAN_PROTOCOL::messageDetails(unsigned long ID, char msg[]) {
  switch (ID) {
    case 0x100: {
      Serial.println("\n\n Battery voltage, current, and Remaining Capacity ");
      
      // Voltage value
      volt = (msg[0] << 8) | msg[1];
      // Current value
      curr = (msg[2] << 8) | msg[3];

      // Remaining capacity
      remCap = (msg[4] << 8) | msg[5];

      Serial.print("\n\n Battery Voltage: ");
      Serial.println(volt);

      Serial.print("\n\n Battery Current: ");
      Serial.println(curr);

      Serial.print("\n\n Remaining Capacity: ");
      Serial.println(remCap);
    } break;

    case 0x101: {
      Serial.println("\n\n Battery Full capacity, number of cycles, and relative rate of discharge");
      
      // Full capacity
     fullCap = (msg[0] << 8) | msg[1];

      // Number of cycles
      nCycles = (msg[2] << 8) | msg[3];

      // Relative rate of discharge
      rSOC = (msg[4] << 8) | msg[5];

      Serial.print("\n\n Battery Full Capacity: ");
      Serial.println( fullCap);

      Serial.print("\n\n Number of Cycles: ");
      Serial.println(nCycles);

      Serial.print("\n\n Relative Rate of Discharge (%): ");
      Serial.println(rSOC);
    } break;

    case 0x105: {
      Serial.println("\n\n Temperature Values, NTC1, NTC2 NTC3 ");
      
      // Temperature values
     uint16_t val1 = (msg[0] << 8) | msg[1];
      uint16_t val2 = (msg[2] << 8) | msg[3];
      uint16_t val3 = (msg[4] << 8) | msg[5];

      uint16_t NT1 = (val1 - 2731) / 10;
      uint16_t NT2 = (val2 - 2731) / 10;
      uint16_t NT3 = (val3 - 2731) / 10;

      Serial.print("\n\n NTC1 Temperature: ");
      Serial.println(NT1);
      Serial.print("\n\n NTC2 Temperature: ");
      Serial.println(NT2);

      Serial.print("\n\n NTC3 Temperature: ");
      Serial.println(NT3);
    } break;
  }
}

CAN_PROTOCOL::CAN_PROTOCOL(int canINT,int canCS): CAN0(canCS)
{
    //setting can pins 
    CAN0_INT = canINT;
    canCS = canCS;
    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.

    if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
        Serial.println("MCP2515 Initialized Successfully!");
    else
        Serial.println("Error Initializing MCP2515...");
    
    CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

    pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
    
    Serial.println("MCP2515 Library Receive Example...");
}
CAN_PROTOCOL can= CAN_PROTOCOL(17, 53);