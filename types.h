// #include <Arduino_FreeRTOS.h>

#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include <Keypad.h>
 #include "newLock.h"
//#include "TouchScreen.h" // only when you want to use touch screen#


// #include <DallasTemperature.h>

// #include <OneWire.h>

#define _SS_MAX_RX_BUFF 311

#include <SoftwareSerial.h>
#include <mcp_can.h>

#include "Fonts/FreeSans9pt7b.h"    // when you want other fonts
#include "Fonts/FreeSans12pt7b.h" // when you want other fonts
#include "Fonts/FreeSerif12pt7b.h" // when you want other fonts
#include "FreeDefaultFonts.h" // when you want other fonts
#include <DHT11.h>
#include <ZMPT101B.h>
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

//setting can pins 
#define CAN0_INT 17
#define CANCS 53

#define relay 10

ZMPT101B voltageSensor1(A8, 50.0);
#define voltageSensor A8
#define currentSensor A9