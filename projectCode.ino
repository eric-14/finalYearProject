#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include <Keypad.h>
//#include "TouchScreen.h" // only when you want to use touch screen#


#include <DallasTemperature.h>

#include <OneWire.h>

#include <ACS712.h>
#include <SoftwareSerial.h>


#include "Fonts/FreeSans9pt7b.h"    // when you want other fonts
#include "Fonts/FreeSans12pt7b.h" // when you want other fonts
#include "Fonts/FreeSerif12pt7b.h" // when you want other fonts
#include "FreeDefaultFonts.h" // when you want other fonts

#include "SPI.h"  // using sdcard for display bitmap image
#include "SD.h"  

#include <ZMPT101B.h>

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

#define oneWireBus 11
#define voltageSensor A8
#define currentSensor A9
#define tempSensor A5
#define relay 10

ZMPT101B voltageSensor1(A8, 50.0);

float sensitivity = 0.100;
int offsetVoltage = 2.5;
int adcVoltage = 0;


int TimeCounter = 0;

int TotalVoltage =0; 
int TotalCurrent = 0; 

float CurrentVoltage = 0; 
float CurrentCurrent =0;

int startTime = millis();

//varaible inidacting start of the program
int startProgram = 0; 


SoftwareSerial _serial(18,19);
void drawScreen();
float readTemperature();

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {22, 24, 26, 28}; //{39, 41, 43, 45};    
byte colPins[COLS] = {30, 32, 34, 36};//{47, 49, 51, 53};

String textInput = "";
int charCounter = 0;
 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
int _voltageSensor = 0;
int _currentSensor = 0;
int currentValue = 0;
int _tempSensor = 0;
bool _startCharging = 0;

float temp1 = 0.0;

int AmountPaid = 0;
String safetyLevel = "G";
String voltage, temp, current=""; 



OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

//(int CS=A3, int RS=A2, int WR=A1, int RD=A0, int RST=A4)
MCUFRIEND_kbv tft(A3, A2, A1, A0, A4); 

void setup()
{ 
                
                Serial.begin(9600);
                sensors.begin();
               

                _serial.begin(9600);
                _serial.println("AT");
                _serial.println("AT+CMGF=1");
              
                uint16_t ID = tft.readID(); 
              // uint16_t identifier = 0x9341;
                Serial.println("Starting--- code ");
                tft.begin(ID);

                Serial.print("TFT ID is : ");
                Serial.print(ID);
                //screen resolution
                uint16_t width = tft.width(); //int16_t width(void);
                uint16_t height = tft.height(); //int16_t height(void); 

                pinMode(voltageSensor, INPUT);
                pinMode(currentSensor, INPUT);
                pinMode(relay, OUTPUT);
                
                Serial.print("Screen resolution is :");
                Serial.print(width);
                Serial.print(" ");
                Serial.print(height);
                voltageSensor1.setSensitivity(SENSITIVITY);
                
             
              


}
              void loop(){
                
                int currentTime = millis();
                
                
                
                char receivedData ;

              

                //get the voltage value of the voltage sensor 
               

                float voltage1 = voltageSensor1.getRmsVoltage();

                Serial.print(" Voltage sensor is >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  " );
                Serial.println(voltage1);

                if(voltage1 > 400){
                    voltage = "0"; 
                    voltage1 = 0;

                }

                voltage = voltage1;
                _currentSensor  = analogRead(currentSensor);
                
                adcVoltage = (_currentSensor / 1023) * 5;
                current= ((  adcVoltage - offsetVoltage ) / sensitivity);
              //display value from current sensor in the screen 

  
                Serial.print(" current sensor is >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  ");
                Serial.println(current);
              
              // _tempSensor = analogRead(tempSensor);
              

                
                Serial.print("current is : >> "); 
                Serial.println(current);
                  if(currentTime - startTime > 200)
                      {

                        TotalVoltage += voltage1; 
                        TotalCurrent += currentValue;
                        TimeCounter  ++;
                      }
  /*
                /*
                  Average current and voltage
                */
                current = TotalCurrent / TimeCounter;
                voltage = TotalVoltage / TimeCounter;


                // sensors.requestTemperatures();
                // temp = sensors.getTempCByIndex(0);

                
                temp1 = readTemperature();

              
                Serial.print("temperature is : >>> "); 
                Serial.print(temp1);
                Serial.println(" C");

                //paramters that must be true to start charging 
                if(_tempSensor < 80){
                  _startCharging = 1; 
                  digitalWrite(relay, HIGH);
                }

                //Stop Charging in case temperature sensor is above room temperature 
                if(_tempSensor > 80 ){
                  // variable controlling charging 
                  _startCharging = 0; 
                  digitalWrite(relay, LOW);
                }

                if(_startCharging){
                  //Serial.println("Charging has Started");
                  digitalWrite(relay, HIGH);
                }
                // Serial.print("Voltage sensor value ");
                // Serial.println(_voltageSensor);

                // Serial.print("current sensor value ");
                // Serial.println(_currentSensor);

                if (startProgram == 0 ){
                    drawScreen(); 
                    //after the frist run don't draw the screen again
                    startProgram = 1; 
                }
                // draw screen for the first time 
                // drawScreen();
                //Screen Functionality code 
                
                char enter = '#';
                char next = '*';
                char clr = 'C';
                char options = 'D';


                char customKey = customKeypad.getKey();
                if (customKey == enter){
                  //voltage = textInput;
                  textInput = "";
                  Serial.println(customKey);
                  
                  Serial.print("Voltage");
                  Serial.println(voltage);
                  drawScreen();
                }
                else if (customKey == next){
                  current = textInput;
                  textInput = "";
                  Serial.println(customKey);
                  
                  Serial.print("current");
                  Serial.println(current);
                  drawScreen();
                }
                else if (customKey == clr){
                  current= "";
                  voltage ="";
                  textInput="";
                  safetyLevel = "";
                  AmountPaid = 0;
                  drawScreen();
                }else if(customKey == options){
                  // Draw page for more settings 
                  Serial.println("Options table ");
                }
                else if(customKey){
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
                tft.println(voltage);


                tft.setCursor(0,90);
                tft.print("Current : >> ");
                tft.println(current);

                tft.setTextSize(2);
                tft.setCursor(0,120);
                tft.print("Battery Safety:>> ");
                tft.println(safetyLevel);

                tft.setCursor(0,200);
                tft.print("Next option press *");
                

                tft.setCursor(0,250);
                tft.print("Amount Paid ");
                tft.println(AmountPaid);

                
                tft.setCursor(0,270);
                tft.print("Temperature:>> ");
                tft.println(temp1);
              
                tft.setTextSize(1.5);
                tft.println("Press C to clr scr");
                tft.setCursor(0, 300);
                tft.print("We're Number 1");

}

float readTemperature(){
   sensors.requestTemperatures();
  // Serial.print("Dallas temp resolution ");

   float temp = 0.0;
  // Serial.println(sensors.getResolution());

   //delay(500);
  // sensors.getWaitForConversion();
    
    temp = sensors.getTempCByIndex(0);
  

    if(temp != -127){
      Serial.print("the temperature is "); 
      Serial.println(temp);
      return temp;
    }else{
      Serial.println("Error occurred when reading temperature value ");
      return -1000.0;
    }
   
    
    
}

