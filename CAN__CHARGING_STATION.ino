// CAN Receive Example
//

#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];


char msgString[128];                        // Array to store serial string
void sendRemoteMessages();

unsigned int messageLen = 8;


#define CAN0_INT 7                             // Set INT to pin 2
MCP_CAN CAN0(53);                               // Set CS to pin 10


void setup()
{
  Serial.begin(115200);
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  
  Serial.println("MCP2515 Library Receive Example...");
}

void loop()
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

void sendRemoteMessages(){

    uint8_t data[8] = { 0x00, 0x01 , 0x02 , 0x03 , 0x04, 0x05 , 0x06, 0x07 };

    
    Serial.println(" Sending CAN Remote Message to Battery ");
    unsigned long canID = 0x11 | 0x40000000; 
    

    
    CAN0.sendMsgBuf(canID, 0, 8,   &data[0]);
    Serial.print(" CAN Message sent "); 
    Serial.println(canID);
    delay(300); 


}
