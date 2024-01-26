void canMain()
{
  Serial.print(" \n \n  Calling CAN Function  \n ");
   /*
    *
     to receive battery information start by sending a remote CAN message
  */
  sendRemoteMessages(canID);
  /**
   * Increment CAN ID from 0x100 to 0x107 to receive different data packets
  */
  canID++;

  if(canID > 0x107){
    canID = 0x100;
  }

  if(digitalRead(CAN0_INT)){
      //Serial.println("CAN INT PIN HIGH 1"); 
  }else if(!digitalRead(CAN0_INT)){
      Serial.println(" CAN LINE LOW  READY TO RECEIVE DATA ");
  }
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {

    Serial.println("      Receiving CAN messages      ");
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    //Serial.println(" printing msg string ");

    if((rxId & 0x80000000) == 0x80000000){     // Determine if ID is standard (11 bits) or extended (29 bits)
      Serial.println("Extended ID:   DLC "); 
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    }else{
      sprintf(msgString, "Standard ID: 0x%.3x    DLC: %1d  Data:", rxId, len);
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
void sendRemoteMessages(unsigned long canID){

   uint8_t data[8] = { 0x00, 0x01 , 0x02 , 0x03 , 0x04, 0x05 , 0x06, 0x07 };
   // Serial.println(" Sending CAN Remote Message to Battery ");
    //unsigned long canID =   0x100;//0x11 | 0x40000000;

    CAN0.sendMsgBuf(canID | 0x40000000, 0, 0, 0);
    
    //CAN0.sendMsgBuf(canID, 0,8,data   );
    Serial.print(" \n \n \t  Sending Remote messages to the battery Waiting Response "); 
    Serial.println(canID);
    delay(100);

}
 void messageDetails(unsigned long ID, char msg[]) {
  switch (ID) {
    case 0x100: {
      Serial.println("\n Battery voltage, current, and Remaining Capacity ");
      
      // Voltage value
      volt = (msg[0] << 8) | msg[1];
      // Current value
      curr = (msg[2] << 8) | msg[3];

      // Remaining capacity
      remCap = (msg[4] << 8) | msg[5];

      Serial.print("\nBattery Voltage: ");
      Serial.println(volt);

      Serial.print("\n Battery Current: ");
      Serial.println(curr);

      Serial.print("\n Remaining Capacity: ");
      Serial.println(remCap);
    } break;

    case 0x101: {
      Serial.println("\nBattery Full capacity, number of cycles, and relative rate of discharge");
      
      // Full capacity
     fullCap = (msg[0] << 8) | msg[1];

      // Number of cycles
      nCycles = (msg[2] << 8) | msg[3];

      // Relative rate of discharge
      rSOC = (msg[4] << 8) | msg[5];

      Serial.print("\nBattery Full Capacity: ");
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

       NT1 = (val1 - 2731) / 10;
       NT2 = (val2 - 2731) / 10;
       NT3 = (val3 - 2731) / 10;

      Serial.print("\n\n NTC1 Temperature: ");
      Serial.println(NT1);
      Serial.print("\n\n NTC2 Temperature: ");
      Serial.println(NT2);

      Serial.print("\n\n NTC3 Temperature: ");
      Serial.println(NT3);
    } break;
  }
}
