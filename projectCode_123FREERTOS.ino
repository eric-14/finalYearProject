#include "types.h"

//CAN VARIABLES
#include "definitions.h"

void canMain();
void sendRemoteMessages(unsigned long canID);
void messageDetails(unsigned long ID, char msg[]);
void drawScreen();
float readTemperature();
void drawScreenBAT();
void fireSuppression();
void updateSerial();
void sendSMS(String Message, String number);

String generatedToken(int seed);
float powerCalculation(int amountPaid);

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

Lock door(41);
DHT11 dht11(40);
void keypad();

//GSM module parameter-s
//pin 19 - rx 
//pin 18 - tx 
SoftwareSerial mySerial(12,11);

//(int CS=A3, int RS=A2, int WR=A1, int RD=A0, int RST=A4)
MCUFRIEND_kbv tft(A3, A2, A1, A0, A4);
int temperature = 0;

int CANtimer = millis();

// CAN_PROTOCOL can(17, 53);
MCP_CAN CAN0(CANCS); 

void setup()
{   // sensors.begin();
    mySerial.begin(9600);
    Serial.begin(9600);
    uint16_t ID = tft.readID(); 
    // uint16_t identifier = 0x9341;
    Serial.println("Starting    code ");
    tft.begin(ID);

    // Serial.print("TFT ID is : ");
    // Serial.print(ID);
    //screen resolution
    uint16_t width = tft.width(); //int16_t width(void);
    uint16_t height = tft.height(); //int16_t height(void);    
    // mySerial.begin(9600);
    Serial.println("Initializing...");
    delay(100);
    Serial.println("<><><><><><<><>>>> AT");
    mySerial.println("AT"); // Sends an ATTENTION command, reply should be OK
    updateSerial();
    delay(100);
    mySerial.println("AT+CPIN?");
    updateSerial();
    delay(500);

    mySerial.println("AT+CPIN=9574");
    updateSerial();
    delay(500);

    Serial.println(">>>>>>>>>>>>>>>> AT+CPIN"); // printing out serial
    Serial.println("  >>>>>>>>>>>>>>>> AT+CREG?  ");
    delay(500);

    mySerial.println("AT+CREG?");
    updateSerial();

    delay(500);

    mySerial.println("AT+CMGF=1"); // Configuration for sending SMS

    delay(500);

    Serial.println("><><><><><><<><>>>> AT+CMFG=1"); // printing out serial for debugging
    mySerial.println("AT+CMGDA=DEL ALL");

    delay(500);

    pinMode(voltageSensor, INPUT);
    pinMode(currentSensor, INPUT);
    pinMode(relay, OUTPUT);
    // Serial.print("Screen resolution is :");
    // Serial.print(width);
    // Serial.print(" ");
    // Serial.print(height);   
  
    voltageSensor1.setSensitivity(SENSITIVITY);
    
    if(CAN0.begin(MCP_ANY, CAN_500KBPS,  MCP_8MHZ) == CAN_OK)
        Serial.println("MCP2515 Initialized Successfully!");
    else
        Serial.println("Error Initializing MCP2515...");
    
    CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

    pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
    digitalWrite(relay, HIGH);
}
void loop()
{           
    //Serial.print()         
  if( rSOC  == 100){
    //battery is full- 
    _startCharging = false;
  }
            
  if (clientPower > 0.0)
  {
    _startCharging = true;
  }
  else
  {
    _startCharging = false;
  }
  if (_startCharging)
  {
    Serial.println("Charging has Started");
    // close the door
    door.lock(0);
    digitalWrite(relay, LOW);
  }
  else if (_startCharging == false)
  {
    door.lock(1);
  }
  if (rSOC == 100)
  {
    _startCharging = false;
  }
  if (temperature > outtemp || NT1 > inttemp || NT2 > inttemp || NT3 > inttemp || NT4 > inttemp || NT5 > inttemp || NT6 > inttemp)
  {
    Serial.println("Fire Suppression has Started");
     _startCharging = false;
    // close the door
    fireSuppression();
  }
  int currentTime = millis();
  // 
  if(currentTime - CANtimer> 2000)
  {
    //run can
    ------------------------canMain();
    CANtimer = currentTime;
    
    updateSerial();
    //mySerial.flush();
  }
  //open the door assume not charging
  // int currentTime = millis();
  char receivedData;
  // get the voltage value of the voltage sensor
  float voltage1 = voltageSensor1.getRmsVoltage();

  if (voltage1 > 400)
  {
    voltage = "0";
    voltage1 = 0;
  }
  voltage = voltage1;
  _currentSensor = analogRead(currentSensor);
  adcVoltage = (_currentSensor / 1023) * 5;
  int current1 = ((adcVoltage - offsetVoltage) / sensitivity);
  if(current1 < 0){current1 = "";}
  current = current1;
  
  temperature = dht11.readTemperature();

  if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT)
  {
    // Serial.print("Temperature: >>>>>>>>>>>");
    // Serial.print(temperature);

  }
  else
  {
    //Serial.println(DHT11::getErrorString(temperature));
    temperature = 0; 
    //Serial.println(" \t \t Temperature is ");
  }
  if(temperature == DHT11::ERROR_TIMEOUT)
  {
    temperature = 0;
  }

  if (startProgram == 0)
  {
    drawScreen();
    // after the frist run don't draw the screen again
    startProgram = 1;
  }
  if(Token && TokenOnce == true)
  {
    drawScreen();
    TokenOnce = false;
  }

  // draw screen for the first time
  // drawScreen();
  // Screen Functionality code
  keypad();
 
}

void keypad(){
  char customKey = customKeypad.getKey();
  if (customKey == enter)
  {
    // voltage = textInput;
    textInput = "";
    drawScreen();
  }
  else if (customKey == next)
  {
    current = textInput;
    textInput = "";
    //Serial.println(customKey);

    drawScreen();
  }
   else if (customKey == 'A')
  {
    current = textInput;
    textInput = "";
    _startCharging = true;
    digitalWrite(relay, LOW);
    //Serial.println(customKey);

    drawScreen();
  }
  else if (customKey == clr)
  {
    current = "";
    voltage = "";
    textInput = "";
    safetyLevel = "";
    AmountPaid = 0;
    drawScreen();
  }
  else if(customKey == battery){
    // Draw page for more settings
    Serial.println("   BATTERY INFORMATION DISPLAY ");
    drawScreenBAT();
  }
  else if (customKey)
  {
    textInput = textInput + customKey;
    Serial.print("Text Input :");
    Serial.println(textInput);
  }
}

void drawScreen(){
    Serial.println(" Drawing screen ");
    tft.setTextColor(BLACK);
    tft.fillScreen(DARKGREY);
    tft.setTextSize(2);
    tft.setCursor(0,0);
    tft.print("Charging Station \n");
    tft.setCursor(0,60);
    tft.print("Voltage : >> ");
    tft.println(String(voltage));
    tft.setCursor(0,90);
    tft.print("Current : >> ");
    tft.println(String(current));
    tft.setTextSize(2);
    tft.setCursor(0,120);
    tft.print("Battery Safety:>> ");
    tft.println(String(safetyLevel));
    tft.setCursor(0,200);
    tft.print("Next option press *");
    tft.setCursor(0,250);
    tft.print("Unique Token ");
    tft.println(String(Token));
    tft.setCursor(0,270);
    tft.print("Temperature:>> ");
    tft.println(String(temperature));
  
    tft.setTextSize(1.5);
    tft.println("Press C to clr scr");
    tft.setCursor(0, 300);
    tft.print("We're Number 1");
}

void drawScreenBAT()
{
  Serial.println(" Drawing CAN ");
  tft.setTextColor(BLACK);
  tft.fillScreen(DARKGREY);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.print("CAN INFORMATION ");
  tft.setCursor(0, 30);
  tft.print("Battery volt: ");
  tft.println(volt);

  tft.setCursor(0, 80);
  tft.print("Battery Curr: ");
  tft.println(curr);

  tft.setTextSize(2);
  tft.setCursor(0, 120);
  tft.print("Bat % charge:> ");
  tft.println(rSOC);

  tft.setCursor(0, 160);
  tft.print("NTC1 :>>");
  tft.println(NT1);

  tft.setCursor(0, 190);
  tft.print("NTC2 :>>");
  tft.println(NT2);

  tft.setCursor(0, 2300);
  tft.print("NTC3 :>> ");
  tft.println(NT3);

  tft.setTextSize(1.5);
  tft.setCursor(0, 250);
  tft.println("Press C to clr scr");
  tft.setCursor(0, 300);
  tft.print("We're Number 1");
}
void movingFN(int duration, bool direction)
{
  Serial.print("  Moving Function ");
  Serial.println(direction);

  unsigned long startTime = millis();

  while (millis() - startTime < duration)
  {
    if (checkAngle == 0)
    {
      a = +1;
    }
    if (checkAngle == 360)
    {
      a = -1;
    }
    angle = angle + a;
    a = angle;
    step = map(a, 0, 360, 0, stepsPerRevolution);

    digitalWrite(dirPin, direction ? HIGH : LOW);

    for (int i = 0; i < step; i++)
    {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
    }
  }
}

void fireSuppression()
{
  Serial.println("    &&&&&&&&&&&  Fire suppression &&  ");
  movingFN(5000, false); // Move in reverse for 5 seconds
  delay(200); 
  movingFN(1000, true);  // Move forward for 5 seconds
  delay(200);            // Pause for 2 seconds
            // Pause for 2 seconds
}

void canMain()
{
  Serial.print(" \n \n  Calling CAN Function  \n ");
   /*
    *
     to receive battery information start by ding a remote CAN message
  
  */
   
  sendRemoteMessages(canID);

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

void updateSerial() {
  String number = "";
  String amountPaid = "";

  char numberBuffer[12]; // Adjust the size as needed
  char amountPaidBuffer[20]; // Adjust the size as needed

  int numberIndex = 0;
  int amountPaidIndex = 0;

  while (mySerial.available()) {

    char buffer[CHUNK_SIZE + 1];
    char messageloc[4]="";

    int bytesRead = mySerial.readBytes(buffer,CHUNK_SIZE);
    Serial.print("printing buffer ");
    Serial.println(buffer);
    //mySerial.flush();
    buffer[bytesRead] = '\0';  // Null-terminate the string
    //+CMTI: "ME",46
    int messIndex = 0;
    

    for(int h=0; h< CHUNK_SIZE; h++){
      if(buffer[h] == '+' and buffer[h+1] == 'C' and buffer[h+2] == 'M' and buffer[h+3] == 'T' and buffer[h+4] == 'I')
      {
        Serial.println("\t \t message loc  ");
        int m =0; 
        // while(m < 20){
        //   if(isdigit(buffer[h+m+4])){
        //       messageloc[messIndex] = buffer[h+m+4];
        //       Serial.print(" buffer[h+m+4]  ");
        //       Serial.println(buffer[h+m+4]);
        //       messIndex++;
        //       m++;
             
        //   }
          
        // }
        messageloc[0] = buffer[h+12];
        messageloc[1] = buffer[h+13];
        messageloc[2]='\0';
        Serial.print(" message loc is ");
        Serial.println(messageloc);
        
      }
      // else{
      //   break;
      // }
      
    }
    if(atoi(messageloc) > 0){
        //checking if there is a message
        //in case of a message print it out
        mySerial.println("AT+CMGR="+String(messageloc)); 
        delay(100);
    }
    // for(int i = 0; i < 4; i++){
    //   messageloc[i] = '';
    // }
   // mySerial.println("AT+CMGR=70");

    // Serial.flush();
    // Serial.print("printing the buffer ");

    
    //give time for the buffer to be read 
   // delay(50);
    // 
    // mySerial.flush();
    // delay(500);
    //mySerial.flush();
    int numberCounter = 0;
    int counter2 = 0;
   // Serial.println("Calling Fnumber here is the answer");
    bool controller = false;
    for (int i = 0; i < CHUNK_SIZE ; i++) {
      if(controller){
        break;
        }
 
      if (buffer[i] == 'N' && buffer[i+1] == 'o' && buffer[i+2] == '.') {
        //Serial.print(" Finding the Number  \n");
        int j = 0;
        int numberWhile = 1;
        while (numberWhile) {
          //  Serial.print(" current buffer value is  when getting number  ");Serial.println(buffer[i + j + 4]);
          if (isdigit(buffer[i + j + 4]) == true   || buffer[i + j + 4] == '.') { //|| buffer[i + j + 4] == ' ' 
             numberBuffer[numberIndex++] = buffer[i + j + 4];
           // number+=(char)buffer[i + j +3];
            //number += buffer[i + j +3];
            j++;
          } else {
            numberBuffer[numberIndex] = '\0';
            Serial.print(" we have the number  ");
            Serial.println(numberBuffer);
            //amountPaid="";
            numberWhile = 0;
            controller = true;
            break;
          }
        }
      }
    }
    //code to confirm number and amount paid have been parsed
    
    //Serial.println("Calling Fmoney Here is the answer ");
    bool loopController = false; 
    for (int i = 0; i < CHUNK_SIZE ; i++) {
      if(loopController){break;}
      bool Bmoney = (buffer[i] == 'K' && buffer[i + 1] == 'E' && buffer[i + 2] == 'S');
      if (Bmoney) {
          // Here's the money
        int j = 0;
        int moneyWhile = 1;
        // Serial.print(" loop money \n");
        while (moneyWhile) {
          // Serial.print(" current money buffer value is    ");Serial.println(buffer[i + j + 5]);
          if (isdigit(buffer[i + j + 5]) == true || buffer[i + j + 5] == '.')  //or buffer[i+j+3] == '.' || buffer[i + j + 5] == ' ' 
          {
            //amountPaid+= (char) buffer[i + j + 4];
            //amountPaid +=  buffer[i + j + 4];
            amountPaidBuffer[amountPaidIndex++] = buffer[i + j + 5];
            j++;
          } else {
            amountPaidBuffer[amountPaidIndex] = '\0';
            Serial.print("We have the money   ");
            Serial.println(amountPaidBuffer);
            j = 0;
            moneyWhile = 0;
            loopController = true; 
            break;
          }
        }
    }
    }
    
    //Serial.println(" after for loops ");
    if (amountPaidIndex > 0 && numberIndex > 0){
        // Serial.println(" checking next if number and money exist ");
           //ready to send token message
        Serial.println(" Replying with token message ");

        clientPower = powerCalculation(atoi(amountPaidBuffer));
        Serial.print(" The client will receive this number of Tokens ");
        Serial.println(clientPower);
        //generating random token
        Token = generatedToken(atoi(amountPaidBuffer) + 45);
   
        TokensGenerated[tokenCounter] = Token;
        tokenCounter++;

        // delay(50);

        Serial.print("Token message  ");
        Serial.println(TokenMessage);
        //send token Message

        String n0 = String(numberBuffer);
    
        if(n0 != previousNumber)
        {
          //Sending the SMS only once 
          sendSMS(TokenMessage,n0);
          Serial.println(" Charging station sent a token ");
          //store the most recent number 
          previousNumber = n0;
        }
        if(millis() - previousTime   > SMSduration)
        {
          //empty the previousNumber
          previousNumber  = "";
        }
        Token = "";

        numberIndex = 0;
      
        amountPaidIndex = 0;
        
    }
    // delay(50);
    //break;

    }
}

void sendSMS(String Message, String number) {
// delay(100);
  String cleanedNumber = "";
  for (int i = 0; i < 12; i++) {
    if (isdigit(number.charAt(i))) {
      cleanedNumber += number.charAt(i);
    }
  }

  Serial.print("cleaned Numnber  ");
  Serial.println(cleanedNumber);

  String TokenMessage = "Hello Welcome to Charging Station 1.\n You have bought power: ";
  String token2 = " W\n Here is the unique token number: ";
  String last =  "\n Thank you from Mwangi & Rispha.";

  mySerial.println("AT+CMGF=1\r\n");

  delay(500);
  mySerial.println("AT+CGMS=17,167,0,0\r");
  delay(500);
  

  mySerial.println("AT+CMGS=\"+"+ cleanedNumber + "\"\r\n");
  delay(500);

  mySerial.print(TokenMessage);
  mySerial.print(String(clientPower));
  mySerial.print(token2);
  mySerial.print(Token);

  //mySerial.print(token2);
  mySerial.print(last);
  mySerial.write(26);  // Ctrl+Z to indicate end of message
  mySerial.println();  // Add a new line to finalize the command

  delay(300);
 // mySerial.flush();
}

float powerCalculation(int amountPaid) {
  // this is the amount of power bought
  float powerBought = 800 * amountPaid / costOfPower;
  Serial.println(" Power has been calculated ");
  return powerBought;
}
String generatedToken(int seed) {
  char rand[5];
  randomSeed((long)seed);
  for (int i = 0; i < 4; i++) {
    rand[i] = '0'+ random(0,10);
    // Serial.print("This is the random string ");
    // Serial.println(rand);
  }
  rand[4] = '\0';

  Serial.print(" Created a random token ready to be keyed in ");
  Serial.println(rand);

  return String(rand);
}
 
