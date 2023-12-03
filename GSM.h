#ifndef GSM_H
#define GSM_H
#include <Arduino.h>


#define _SS_MAX_RX_BUFF 256
#include "SoftwareSerial.h"

extern SoftwareSerial mySerial;
extern String cmd ;


extern String previousNumber;

extern int SMSduration;
 
extern int previousTime ;

extern float clientPower ;
extern String Token;
float powerCalculation(int amountPaid);
String generatedToken(int seed);


//stores the last 10 tokens that the system has generated
extern String TokensGenerated[20];

extern String TokenMessage;

const int CHUNK_SIZE = 200;  // Adjust the chunk size based on your needs

//Tracker of the latest token
extern int tokenCounter ;

extern int costOfPower ;
// this is the amount paid by the cl


extern char buffer[CHUNK_SIZE + 1];

void updateSerial();

void sendSMS(String Message,String number);

void receiveSMS();
void printResponse();

#endif