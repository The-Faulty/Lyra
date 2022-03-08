/*
  Lyra barometric rocketry deployment computer
*/

#include <Arduino.h>
#include <vars.h>

void setup() {
  state = startUp;

  flash.begin();
  SD.begin(SD_CS);

  while (baro.connect() > 0) {
    //indicate lack of connection like beeps
    delay(100);
  }
  baro.checkUpdates();
  while (!baro.isReady()); // just waiting for the readings
  gndPres = baro.GetPres();
  baro.setPOffset(gndPres);

  pinMode(VOLT, INPUT);
  
  state = idle;
}

void loop() {
  period = millis() - lastMillis;
  lastMillis = millis();

  baro.checkUpdates();
  if (baro.isReady()) {
    if (state == idle)
    {
      readAlt();
      //should I put some logging in here? not really sure, I'll come back to it
      if (avgAlt > MINALT) state = liftOff;
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
}

void readAlt() {
  curAlt = baro.getAltitude(true);
  rollAvg[rollIndex] = curAlt;
  if (rollIndex >= 9) rollIndex = 0;
  else rollIndex++;

  for (int i = 0; i < ROLLAVGLENG - 1; i++) {
    avgAlt += rollAvg[i];
  }
  avgAlt /= ROLLAVGLENG - 1;
}

void calcRate() {
  /* for (int i = rollIndex; i < ROLLAVGLENG - 1; i++) {
    if (rollIndex > 0 && i + 1 >= ROLLAVGLENG - 1) {
      avgRate += (rollAvg[0] - rollAvg[i]) / (period / 1000);
      i = 0;
    } else {
      avgRate += (rollAvg[i + 1] - rollAvg[i]) / (period / 1000);
    }
  }
  I think the below code will work better than what I was trying ot figure out above */
  for (int i = 0; i < ROLLAVGLENG - 1; i++) {
    if (i + 1 == rollIndex) {
      avgRate += (rollAvg[0] - rollAvg[ROLLAVGLENG - 1]) / (period / 1000);
    } else {
      avgRate += (rollAvg[i + 1] - rollAvg[i]) / (period / 1000);
    }
  }
  avgRate /= ROLLAVGLENG - 2;
}

float getBattVoltage() {   //This doesnt need to be a function, but I think it will aid in code readability so why not
  return analogRead(VOLT);
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
    curPress,
    curAlt,
    avgAlt,
    avgRate,
    state,
    getBattVoltage(),
    millis()
  };

  flash.writeAnything(flashIndex, d);
  flashIndex += sizeof(d);
}