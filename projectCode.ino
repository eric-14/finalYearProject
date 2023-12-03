#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include <Keypad.h>
#include <mcp_can.h>
#include <DHT11.h>
#include <ZMPT101B.h>

#include <SoftwareSerial.h>

#include "Fonts/FreeSans9pt7b.h"   // when you want other fonts
#include "Fonts/FreeSans12pt7b.h"  // when you want other fonts
#include "Fonts/FreeSerif12pt7b.h" // when you want other fonts
#include "FreeDefaultFonts.h"      // when you want other fonts


#include "SPI.h" // using sdcard for display bitmap image
// #include "SD.h"

#include "newLock.h"
#include "CAN_COMMUNICATION.h"
// #include "GSM.h"

#define SENSITIVITY 500.0f

#define BLACK 0x0000
#define NAVY 0x000F
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define LIGHTGREY 0xC618
#define DARKGREY 0x7BEF
#define BLUE 0x001F
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFD20
#define GREENYELLOW 0xAFE5
#define PINK 0xF81F

#define tempSensor A5
#define relay 10

ZMPT101B voltageSensor1(A8, 50.0);
#define voltageSensor A8
#define currentSensor A9
// Temperature that determine thermal runaway
int inttemp = 65;
int outtemp = 80;

float sensitivity = 0.100;
int offsetVoltage = 2.5;
int adcVoltage = 0;
int TimeCounter = 0;
int TotalVoltage = 0;
int TotalCurrent = 0;
float CurrentVoltage = 0;
float CurrentCurrent = 0;
int startTime = millis();
// varaible inidacting start of the program
int startProgram = 0;

int dirPin = 15;  // CW+ 
int stepPin = 20; //14; CLk+ 
int enPin = 16;  // EN+ 

int stepsPerRevolution = 1600; // for 1/8 step mode

int angle = 0;
int step = 0;
int stepDelay = 100;
int checkAngle = 0;
int a = 0;
bool direction = true; // true for forward, false for reverse

void drawScreen();
float readTemperature();
void drawScreenBAT();
void fireSuppression();

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {22, 24, 26, 28}; //{39, 41, 43, 45};
byte colPins[COLS] = {30, 32, 34, 36}; //{47, 49, 51, 53};

String textInput = "";
int charCounter = 0;

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int _voltageSensor = 0;
int _currentSensor = 0;
int currentValue = 0;
int _tempSensor = 0;
bool _startCharging = false;
bool _fireSuppression = false;

float temp1 = 0.0;

int AmountPaid = 0;
String safetyLevel = "G";
String voltage, temp, current = "";

DHT11 dht11(38);
Lock door(41);



char enter = '#';
char next = '*';
char clr = 'C';
char battery = 'B';
char options = 'D';

//(int CS=A3, int RS=A2, int WR=A1, int RD=A0, int RST=A4)
MCUFRIEND_kbv tft(A3, A2, A1, A0, A4);
int temperature = 0;

// CAN_PROTOCOL can(17, 53);


void setup()
{

  Serial.begin(9600);

  uint16_t ID = tft.readID();
  // uint16_t identifier = 0x9341;
  Serial.println("Starting--- code ");
  tft.begin(ID);

  Serial.print("TFT ID is : ");
  Serial.print(ID);
  // screen resolution
  uint16_t width = tft.width();   // int16_t width(void);
  uint16_t height = tft.height(); // int16_t height(void);

  Serial.print("Screen resolution is :");
  Serial.print(width);
  Serial.print(" ");
  Serial.print(height);
  voltageSensor1.setSensitivity(SENSITIVITY);

  pinMode(voltageSensor, INPUT);
  pinMode(currentSensor, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);

  // mySerial.begin(9600);
  // Serial.println("Initializing...");
  // delay(100);
  // Serial.println("<><><><><><<><>>>> AT");
  // mySerial.println("AT"); // Sends an ATTENTION command, reply should be OK
  // updateSerial();
  // delay(100);
  // mySerial.println("AT+CPIN?");
  // updateSerial();
  // delay(100);

  // mySerial.println("AT+CPIN=9574");
  // updateSerial();
  // delay(100);

  // Serial.println(">>>>>>>>>>>>>>>> AT+CPIN"); // printing out serial
  // Serial.println("  >>>>>>>>>>>>>>>> AT+CREG?  ");
  // delay(100);

  // mySerial.println("AT+CREG?");
  // updateSerial();

  // delay(100);

  // mySerial.println("AT+CMGF=1"); // Configuration for sending SMS

  // delay(100);

  // Serial.println("><><><><><><<><>>>> AT+CMFG=1"); // printing out serial for debugging
}
void loop()
{

  // if (clientPower > 0.0)
  // {
  //   _startCharging = true;
  // }
  // else
  // {
  //   _startCharging = false;
  // }
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
  if (can.rSOC == 100)
  {
    _startCharging = false;
  }
  if (temperature > outtemp || can.NT1 > inttemp || can.NT2 > inttemp || can.NT3 > inttemp || can.NT4 > inttemp || can.NT5 > inttemp || can.NT6 > inttemp)
  {
    Serial.println("Fire Suppression has Started");
    // close the door
    fireSuppression();
  }

  //run can
  can.canMain();
  //open the door assume not charging

  // previousTime = millis();
  // updateSerial();
  // delay(100);

  // int currentTime = millis();
  char receivedData;
  // get the voltage value of the voltage sensor
  float voltage1 = voltageSensor1.getRmsVoltage();

  Serial.print(" Voltage sensor is >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  ");
  Serial.println(voltage1);

  if (voltage1 > 400)
  {
    voltage = "0";
    voltage1 = 0;
  }

  voltage = voltage1;
  _currentSensor = analogRead(currentSensor);

  adcVoltage = (_currentSensor / 1023) * 5;
  current = ((adcVoltage - offsetVoltage) / sensitivity);
  // display value from current sensor in the screen

  Serial.print(" current sensor is >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  ");
  Serial.println(current);

  temperature = dht11.readTemperature();
  if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT)
  {
    Serial.print("Temperature: >>>>>>>>>>>");
    Serial.print(temperature);
    Serial.println(" °C");
  }
  else
  {
    Serial.println(DHT11::getErrorString(temperature));
  }

  if (startProgram == 0)
  {
    drawScreen();
    // after the frist run don't draw the screen again
    startProgram = 1;
  }

  // draw screen for the first time
  // drawScreen();
  // Screen Functionality code

  char customKey = customKeypad.getKey();
  if (customKey == enter)
  {
    // voltage = textInput;
    textInput = "";
    Serial.println(customKey);

    Serial.print("Voltage");
    Serial.println(voltage);
    drawScreen();
  }
  else if (customKey == next)
  {
    current = textInput;
    textInput = "";
    Serial.println(customKey);

    Serial.print("current");
    Serial.println(current);
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
    Serial.println("-BATTERY INFORMATION DISPLAY ");
    drawScreenBAT();
  }
  else if (customKey)
  {
    textInput = textInput + customKey;
    Serial.print("Text Input :");
    Serial.println(textInput);
  }
}

void drawScreen()
{
  Serial.println(" Drawing screen ");
  tft.setTextColor(BLACK);

  tft.fillScreen(DARKGREY);

  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.print("Charging Station \n");

  tft.setCursor(0, 60);
  tft.print("Voltage : >> ");
  tft.println(String(voltage));

  tft.setCursor(0, 90);
  tft.print("Current : >> ");
  tft.println(String(current));

  tft.setTextSize(2);
  tft.setCursor(0, 120);
  tft.print("Battery Safety:>> ");
  tft.println(String(safetyLevel));

  tft.setCursor(0, 200);
  tft.print("Next option press *");

  tft.setCursor(0, 250);
  tft.print("Amount Paid ");
  tft.println(String(AmountPaid));

  tft.setCursor(0, 270);
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

  tft.setCursor(0, 60);
  tft.print("BATTERY VOLTAGE : >> ");
  tft.println(can.volt);

  tft.setCursor(0, 90);
  tft.print("BATTERY Current : >> ");
  tft.println(can.curr);

  tft.setTextSize(2);
  tft.setCursor(0, 120);
  tft.print("Battery % charge:>> ");
  tft.println(can.rSOC);

  tft.setCursor(0, 200);
  tft.print("NTC1 :>>");
  tft.println(can.NT1);

  tft.setCursor(0, 250);
  tft.print("NTC2 :>>");
  tft.println(can.NT2);

  tft.setCursor(0, 270);
  tft.print("NTC3 :>> ");
  tft.println(can.NT3);

  tft.setTextSize(1.5);
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
  movingFN(1000, true);  // Move forward for 5 seconds
  delay(500);            // Pause for 2 seconds
  movingFN(5000, false); // Move in reverse for 5 seconds
  delay(1000);           // Pause for 2 seconds
}