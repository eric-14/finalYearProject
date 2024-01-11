uint16_t volt, curr , remCap = 0;
uint16_t fullCap, rSOC , nCycles = 0;
uint16_t NT1, NT2 , NT3 = 0;
uint16_t NT4, NT5 , NT6 = 0;
long unsigned int rxId =0 ;
unsigned char len = 0;
unsigned char rxBuf[8];

unsigned long canID = 0x100;

char msgString[128];                        // Array to store serial string

unsigned int messageLen = 8;

const int CHUNK_SIZE = 350; 

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
int stepPin = 46; //14; CLk+ 
int enPin = 16;   //16;  // EN+ 

int stepsPerRevolution = 1600; // for 1/8 step mode

int angle = 0;
int step = 0;
int stepDelay = 100;
int checkAngle = 0;
int a = 0;
bool direction = true; // true for forward, false for reverse


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



int outtemp = 80;
int inttemp = 65;


String cmd = "";
String previousNumber = "";
String Token = "";
String TokensGenerated[20] = {};
String TokenMessage = "";

bool TokenOnce = true;

float clientPower = 0.0;


char buffer[CHUNK_SIZE + 1];

 int SMSduration= 5000;

int previousTime= 0;

int tokenCounter = 0;
int costOfPower = 160;

char enter = '#';
char next = '*';
char clr = 'C';
char battery = 'B';
char options = 'D';


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