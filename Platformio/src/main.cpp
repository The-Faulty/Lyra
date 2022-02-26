/*
  Lyra barometric rocketry deployment computer
*/

#include <Arduino.h>
#include <vars.h>

void setup() {
  state = startUp;

  flash.begin();
  SD.begin(SD_CS);

  
}

void loop() {
  if (state == idle)
  {
    readAlt();
    //should I put some logging in here? not really sure, I'll come back to it
    if (avgAlt > minAlt) state = liftOff;
  }
  else if (state == liftOff) {
    readAlt();
    logData();
    //detect burnout
  }
  else if (state == burnOut)
  {
    readAlt();
    if (isApogee());
  }
  else if (state == apogee) {
    //should this really be a state? maybe a boolean would fit better idk
  }
  else if (state == descent) {
    //do stuff for average descent rate ---- make a descent rate function?
  }
  else if (state == drogue) {
    //more descent rate calculations
  }
  else if (state == mains) {
    //some detect landing function
  }
}

void readAlt() {
  curAlt = bmp.readAltitude(gndPres);
  rollAvg[rollIndex] = curAlt;
  if (rollIndex == 9) rollIndex = 0;
  else rollIndex++;

  for (int i = 0; i < rollAvgLeng; i++) {
    avgAlt += rollAvg[i];
  }
  avgAlt /= 10;
}

bool isApogee() {
  if (avgAlt > maxAlt) maxAlt = avgAlt;
  if (avgAlt < maxAlt) timesLessThan += 1;
  else timesLessThan = 0;
  if (timesLessThan >= 10) return true;
  else return false;
}

void logData() {
  
  Logger d = {
    curAlt,
    avgAlt,
    0, //battery voltage tbd
    state,
    millis()
  };

  flash.writeAnything(flashIndex, d);
  flashIndex += sizeof(d);
}