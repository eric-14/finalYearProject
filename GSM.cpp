#include "GSM.h"

//pin 19 - rx 
//pin 18 - tx 
SoftwareSerial mySerial(19, 18);
String cmd = "";
String previousNumber = "";
String Token = "";
String TokensGenerated[20] = {};
String TokenMessage = "";
float clientPower = 0.0;
char buffer[CHUNK_SIZE + 1];

 int SMSduration= 5000;

int previousTime= 0;

int tokenCounter = 0;
int costOfPower = 160;
void updateSerial() {
  String number = "";
  String amountPaid = "";

  char numberBuffer[12]; // Adjust the size as needed
  char amountPaidBuffer[20]; // Adjust the size as needed

  int numberIndex = 0;
  int amountPaidIndex = 0;

  while (mySerial.available()) {
    //char buffer[CHUNK_SIZE + 1];
    int bytesRead = mySerial.readBytes(buffer, CHUNK_SIZE);
    // mySerial.flush();
    buffer[bytesRead] = '\0';  // Null-terminate the string
                               // Serial.println("printing the buffer ");
    Serial.print(buffer);
    //give time for the buffer to be read 
    delay(50);
    Serial.flush();
    mySerial.flush();
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
      bool Bmoney = (buffer[i] == 'K' && buffer[i + 1] == 's' && buffer[i + 2] == 'h');
      if (true == Bmoney) {
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

        delay(100);

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

        numberIndex = 0;
      
        amountPaidIndex = 0;
        
    }
    delay(600);
    break;

    }
}


void sendSMS(String Message, String number) {
  delay(100);
  String cleanedNumber = "";
  for (int i = 0; i < 12; i++) {
    if (isdigit(number.charAt(i))) {
      cleanedNumber += number.charAt(i);
    }
  }

  delay(100);
  Serial.print("cleaned Numnber  ");
  Serial.println(cleanedNumber);

  String TokenMessage = "Hello Welcome to Charging Station 1.\n You have bought power: ";
  String token2 = " W\n Here is the unique token number: ";
  String last =  "\n Thank you from Mwangi & Rispha.";

  mySerial.println("AT+CMGF=1\r\n");

  delay(100);
  mySerial.println("AT+CGMS=17,167,0,0\r");
  delay(500);
  

  mySerial.println("AT+CMGS=\"+"+ cleanedNumber + "\"\r\n");
  delay(1000);

  mySerial.print(TokenMessage);
  mySerial.print(String(clientPower));
  mySerial.print(token2);
  mySerial.print(Token);

  //mySerial.print(token2);
  mySerial.print(last);
  mySerial.write(26);  // Ctrl+Z to indicate end of message
  mySerial.println();  // Add a new line to finalize the command

  delay(100);
  mySerial.flush();
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
 