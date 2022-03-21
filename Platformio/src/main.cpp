/*
  Lyra barometric rocketry deployment computer
*/

#include <Arduino.h>
#include <vars.h>

void setup() {
  state = startUp;

  pinMode(VOLT, INPUT);
  pinMode(BUZZER, OUTPUT);

  //USING DIGITAL WRITE TO CONTROL AN LED THAT WAS BODGED ONTO THE PIN
  //tone(BUZZER, NOTE_E7, 200);
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);

  do {
    //tone(BUZZER, NOTE_C7, 200);
    digitalWrite(BUZZER, HIGH);
    delay(500);
    digitalWrite(BUZZER, LOW);
    delay(500);
  } while (baro.connect() > 0);

  baro.setSamples(MS5xxx_CMD_ADC_2048);
  baro.setPressPa();                      //Set pressure readings to pascals (my preference and needed for the offset)
  barometer.setDelay(10);

  //Set the launch site offset
  baro.checkUpdates();
  while (!baro.isReady()); // just waiting for the readings
  gndPres = baro.GetPres();
  baro.setPOffset(gndPres);

  flash.begin();
  SD.begin(SD_CS);

  tone(BUZZER, NOTE_E7, 100);
  delay(100);
  tone(BUZZER, NOTE_E7, 100);

  //Startup Complete!

  state = idle;
}

void loop() {
  period = millis() - lastMillis;
  lastMillis = millis();

  if (millis() - lastReadingMillis >= 40) { 
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
        if (isApogee()) state = apogee;
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
} 

void readAlt() {
  curAlt = baro.getAltitude(true);
  avgAlt += curAlt;

  int bufferAlt[ROLLAVGLENG], bufferPeriod[ROLLAVGLENG];
  bufferAlt[0] = curAlt;
  bufferPeriod[0] = millis() - lastReadingMillis;

  for (int i = 0; i < ROLLAVGLENG; i++) {
    bufferAlt[i + 1] = rollAvg[i];
    bufferPeriod[i + 1] = rollPeriod[i];
    avgAlt += rollAvg[i];
  }
  memcpy(rollAvg, bufferAlt, sizeof(bufferAlt));
  memcpy(rollPeriod, bufferPeriod, sizeof(bufferPeriod));

  avgAlt /= ROLLAVGLENG - 1;
}

void calcRate() {
  for (int i = 1; i < ROLLAVGLENG - 1; i++) {
    avgRate += (rollAvg[0] - rollAvg[ROLLAVGLENG - 1]) / (rollPeriod[i] / 1000);
  }

  avgRate /= ROLLAVGLENG - 1;
}

void calcAccel() {
  for (int i = 1; i < ROLLAVGLENG - 2; i++) {
    avgAccel += (rollVelo[0] - rollVelo[ROLLAVGLENG - 1]) / (((rollPeriod[i] + rollPeriod[i + 1]) / 2) / 1000);
  }

  avgAccel /= ROLLAVGLENG - 2;
}

byte getBattVoltage() {   //This doesnt need to be a function, but I think it will aid in code readability so why not
  return (byte) map(analogRead(VOLT), 0, 1023, 0, 8.4) * 10);
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
    (int) (curPress * 1000),
    (int) (curAlt * 1000),
    (int) (avgAlt * 1000),
    (int) (avgRate * 1000),
    (int) (avgAccel * 1000),
    state,
    getBattVoltage(),
    millis()
  };

  flash.writeAnything(flashIndex, d);
  flashIndex += sizeof(d);
}