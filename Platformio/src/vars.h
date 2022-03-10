#include <Wire.h>
#include <SPI.h>
#include <MS5x.h>
#include <SPIMemory.h>
#include <SD.h>
#include <SDU.h>
#include <errors.h>

#define CONT1 A3
#define VOLT A4
#define FL_CS 4
#define PY1 5
#define BUZZER 7
#define SD_CS 10

MS5x baro(&Wire);
SPIFlash flash;
File myFile;

//User defined variables
#define MINALT 200                                                          //Minimum altitude for flight detection
#define ROLLAVGLENG 10                                                      //Defines the length of the roling average
#define ISDROGUE false                                                      //Set true if using Lyra to deploy drogue chutes
#define ISAPOGEEDEPLOY false                                                //Set true if you want Lyra to deploy parachutes at apogee
#define DEPLOYALT 500                                                       //Sets the deployment altitude, ignore if using apogee deployment
#define SAVEDATA false                                                      //Mainly for testing purposes

int period;                                                                 //Period between program loops
int lastMillis;                                                             //Last time loop was run
int rollPeriod[ROLLAVGLENG];                                                //Period between readings for velocity purposes
int lastReadingMillis;                                                      //Last time a reading was taken

float gndPres;                                                              //Pressure at the launch site ground
float curAlt;                                                               //Current altitude
float curPress;                                                             //Current pressure
float rollAvg[ROLLAVGLENG];                                                 //Used to take a roling average of altitude data
float maxAlt;                                                               //Stores the max altitude 

float avgAlt;                                                               //Altitude after using the rolling average
float avgRate;                                                              //Average Verticale speed
int rollIndex = 0;                                                          //Used to loop through the 
int timesLessThan = 0;                                                      //Used to ensure apogee has been reached, when over 10

uint32_t flashIndex = 0;                                                    //Index where to write data to flash

enum stateMachine {                                                         //State machine 
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
stateMachine state;

#pragma pack(1)                                                             //tells compiler to pack the variables together with no weird spacing issues that there would otherwise be

struct LoggerVars {                                                         //Struct for data logging
  float pressure;
  float alt;
  float filtAlt;
  float rate;
  int curState;
  float battVolt;
  uint32_t time;
};

union Logger {
  LoggerVars vars;
  char raw[sizeof(LoggerVars)];
};

#pragma pack(0)                                                             //compiler can go back to normal