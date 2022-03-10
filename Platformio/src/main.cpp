/*
  Lyra barometric rocketry deployment computer
*/

#include <Arduino.h>
#include <vars.h>

void setup() {
  state = startUp;

  pinMode(VOLT, INPUT);
  pinMode(BUZZER, OUTPUT);

  flash.begin();
  SD.begin(SD_CS);

  while (baro.connect() > 0) {
    tone(NOTE_c4);
    delay(250);
  }
  baro.setSamples(MS5XXX_CMD_ADC_4096);   //This is the lib's default setting, but I have this to confirm that is the setting
  baro.setPressPa();    //Set pressure readings to pascals (my preference and needed for the offset)
  //Set the launch site offset
  baro.checkUpdates();
  while (!baro.isReady()); // just waiting for the readings
  gndPres = baro.GetPres();
  baro.setPOffset(gndPres);

  tone(BUZZER, NOTE_E4, 100);
  delay(100);
  tone(BUZZER, NOTE_E4, 100);

  //Startup Complete!

  state = idle;
}

void loop() {
  period = millis() - lastMillis;
  lastMillis = millis();

  if (millis() - lastReadingMillis >= 100) { 
    baro.checkUpdates();
    lastReadingMillis = millis();
  }
  if (baro.isReady()) {
    readAlt();
    calcRate();
    switch (state) {
      case idle:
        //should I put some logging in here? not really sure, I'll come back to it
        if (avgAlt > MINALT) state = liftOff;
        break;
      case liftOff:
        logData();
        //detect burnout
        break;
      case burnOut:
        if (isApogee());
        break;
      case apogee:
        //should this really be a state? maybe a boolean would fit better idk
        break;
      case descent:
        //do stuff for average descent rate ---- make a descent rate function?
        break;
      case drogue:
        //more descent rate calculations
        break;
      case mains:
        //some detect landing function
        break;
      case landed:
        //finalize stuff
        break;
    }
} 

void readAlt() {
  curAlt = baro.getAltitude(true);
  /*rollAvg[rollIndex] = curAlt;
  if (rollIndex >= 9) rollIndex = 0;
  else rollIndex++;*/

  int bufferArray[ROLLAVGLENG];
  bufferArray[0] = curAlt;

  for (int i = 0; i < ROLLAVGLENG - 1; i++) {
    bufferArray[i + 1] = rollAvg[i];
  }
  memcpy(rollAvg, bufferArray, sizeOf(bufferArray));

  /*for (int i = 0; i < ROLLAVGLENG - 1; i++) {
    avgAlt += rollAvg[i];
  }
  avgAlt /= ROLLAVGLENG - 1;*/
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
  /*
  ______________________________________________________________
  Once again need to rewrite this, change to using lastReadingMillis and write for not using the 
  */

  avgRate /= ROLLAVGLENG - 1;
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