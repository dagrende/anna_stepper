#include <EEPROM.h>

#include <Bounce2.h>
#include <math.h>

#define SLOW_BUTTON 2
#define FAST_BUTTON 3
#define DIR 16
#define STEP 10
#define up0 9
#define up1 8
#define up2 7
#define up3 6

Bounce slowButtonDebouncer = Bounce();
Bounce fastButtonDebouncer = Bounce();
int speed = 0;
unsigned long stepPeriod;
unsigned long lastStepTime = micros();
unsigned long buttonDownTime;
unsigned long repeatTime;
boolean slowButtonDown = false;
boolean fastButtonDown = false;
boolean unsavedChange = false;
unsigned long changedTime;
byte upPattern[] = {
  B1000,
  B1100,
  B0100,
  B0110,
  B0010,
  B0011,
  B0001,
  B1001};
byte upPhase = 0;

// convert speed index to microsecond period to get equal 
// relative difference between speeds regardleess of speed
unsigned long speedToPeriod(int speed) {
  int absSpeed = abs(speed) - 1;
  return (unsigned long)(1000000 / exp(absSpeed * log(1.3)) );
}

void setup() {
  pinMode(SLOW_BUTTON, INPUT_PULLUP);
  slowButtonDebouncer.attach(SLOW_BUTTON);
  slowButtonDebouncer.interval(10);
  
  pinMode(FAST_BUTTON, INPUT_PULLUP);
  fastButtonDebouncer.attach(FAST_BUTTON);
  fastButtonDebouncer.interval(10);
  
  // dir-step outputs
  pinMode(DIR, OUTPUT);
  digitalWrite(DIR, speed > 0);
  pinMode(STEP, OUTPUT);
  digitalWrite(STEP, 0);
  
  //unipolar outputs
  pinMode(up0, OUTPUT);
  pinMode(up1, OUTPUT);
  pinMode(up2, OUTPUT);
  pinMode(up3, OUTPUT);
  
  EEPROM.get(0, speed);
  if (speed < -32000) {
    speed = 0;
  }
  digitalWrite(DIR, speed > 0);

  stepPeriod = speedToPeriod(speed);
  
  Serial.begin(9600);
}

void addSpeed(int change) {
  speed += change;
  stepPeriod = speedToPeriod(speed);
  digitalWrite(DIR, speed > 0);
  Serial.println(speed);
  changedTime = millis();
  unsavedChange = true;
}

void loop() {
  unsigned long millisTime = millis();
  if (speed != 0) {
    unsigned long time = micros();
    if (time - lastStepTime > stepPeriod) {
      digitalWrite(STEP, 1);
      digitalWrite(STEP, 0);

      byte upOut = upPattern[7 & upPhase++];
      digitalWrite(up0, 1 & upOut); upOut >>= 1;
      digitalWrite(up1, 1 & upOut); upOut >>= 1;
      digitalWrite(up2, 1 & upOut); upOut >>= 1;
      digitalWrite(up3, 1 & upOut); upOut >>= 1;

      lastStepTime = time;
    }
  }
  if (unsavedChange && millisTime - changedTime > 15000) {
    Serial.println("save");
    EEPROM.put(0, speed);
    unsavedChange = false;
  }
  if (slowButtonDown && millisTime - buttonDownTime > 1000) {
    if (millisTime - repeatTime > 100) {
      addSpeed(-1);
      repeatTime = millisTime;
    }
  }
  if (fastButtonDown && millisTime - buttonDownTime > 1000) {
    if (millisTime - repeatTime > 100) {
      addSpeed(1);
      repeatTime = millisTime;
    }
  }
  if (slowButtonDebouncer.update()) {
    if (!slowButtonDebouncer.read()) {
      buttonDownTime = repeatTime = millisTime;
      slowButtonDown = true;
      addSpeed(-1);
    } else {
      slowButtonDown = false;
    }
  }
  if (fastButtonDebouncer.update()) {
    if (!fastButtonDebouncer.read()) {
      buttonDownTime = repeatTime = millisTime;
      fastButtonDown = true;
      addSpeed(1);
    } else {
      fastButtonDown = false;
   }
  }
}
