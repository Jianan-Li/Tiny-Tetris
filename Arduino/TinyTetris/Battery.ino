// Battery level (Fully charged battery = 52)
#define SHUTDOWN_BATT_VAL 41
#define CLEAR_FLAG_BATT_VAL 44
#define LOW_BATT_COUNTER_LIMIT 30  // in seconds

uint8_t lowBattCounter=0;
bool lowBattShutdown=0;

const uint8_t lowBatt1[16]={
  B11111110,
  B11000011,
  B11000001,
  B11000011,
  B11111110,
  B00000000,
  B00000000,
  B00000100,
  B00001000,
  B00011000,
  B00110000,
  B01111110,
  B00001100,
  B00011000,
  B00010000,
  B00100000
};

const uint8_t lowBatt2[16]={
  B11111110,
  B10000011,
  B10000001,
  B10000011,
  B11111110,
  B00000000,
  B00000000,
  B00000100,
  B00001000,
  B00011000,
  B00110000,
  B01111110,
  B00001100,
  B00011000,
  B00010000,
  B00100000
};

void checkBattFlag() {
  if (EEPROM.read(0)==1) {
//    Serial.println("low batt flag was set");
    uint8_t val = analogRead(BATT_PIN)>>4;
//    Serial.print("battery level: ");
//    Serial.println(val);
    
    if (val>CLEAR_FLAG_BATT_VAL) {
//      Serial.println("clear flag");
      EEPROM.write(0,0);
    }
    else {
//      Serial.println("low batt, shutting down");
      shutDown(0);
    }
  }
//  else Serial.println("normal boot up");
}

void monitorBattery() {
  uint8_t val = analogRead(BATT_PIN)>>4;  // 10-4=6 bit resolution
  if (val<SHUTDOWN_BATT_VAL) lowBattCounter+=1;
  else lowBattCounter=0;
  if (lowBattCounter>=LOW_BATT_COUNTER_LIMIT) lowBattShutdown=1;
  Serial.println(val);
//  Serial.print("  ");
//  Serial.println(lowBattCounter);
}

void shutDown(bool writeToEEPROM) {
  // disable only the timer1 overflow interrupt (should not use cli())
  TIMSK1 &= ~_BV(TOIE1);
  if (writeToEEPROM) {
//    Serial.println("setting low batt flag");
    EEPROM.write(0,1);
  }
  for (uint8_t i=0;i<3;i++) {
    ht.writeToDisplay((uint8_t*)lowBatt1); delay(500);
    ht.writeToDisplay((uint8_t*)lowBatt2); delay(500);
  }
  digitalWrite(KILL_PIN,HIGH);
}

void checkLowBatt() {
  if (lowBattShutdown) shutDown(1);
}
