#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>
#include <SPIMemory.h>
#include <SD.h>
#include <errors.h>

#define CONT1 A3
#define VOLT A4
#define FL_CS 4
#define PY1 5
#define BUZZER 7
#define SD_CS 10

Adafruit_BMP3XX bmp;
SPIFlash flash;
File myFile;

float gndPres;                                                              //Pressure at the launch site ground
float curAlt;                                                               //Current altitude
float rollAvg[10];                                                          //Used to take a roling average of altitude data
float maxAlt;                                                               //Stores the max altitude 
float minAlt = 200;                                                         //Minimum altitude for flight detection
float avgAlt;

int rollAvgLeng = 10;                                                       //Defines the length of the roling average
int rollIndex = 0;
int timesLessThan = 0;                                                      //Used to ensure apogee has been reached, when over 10

bool hasDrogue = false;                                                     //Will read the settings from sd card -- will there be a drogue period
bool isMains = true;                                                        //is the computer deploying mains or drogue -- default true

uint32_t flashIndex = 0;                                                    //Index where to write data to flash

enum stateMacine {                                                          //State machine 
  startUp,
  idle,
  liftOff,
  burnOut,
  apogee,
  descent,
  drogue,
  mains,
  landed
};
int state;



#pragma pack(1)                                                             //tells compiler to pack the variables together with no weird spacing issues that there would otherwise be

struct LoggerVars {                                                         //Struct for data logging
  float alt;
  float filtAlt;
  float battVolt;
  int curState;
  uint32_t time;
};

union Logger {
  LoggerVars vars;
  char raw[sizeof(LoggerVars)];
};

#pragma pack(0)                                                             //compiler can go back to normal