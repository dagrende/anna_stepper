#include <Bounce2.h>

#define SLOW_BUTTON 2
#define FAST_BUTTON 3
#define DIR 16
#define STEP 10

Bounce slowButtonDebouncer = Bounce();
Bounce fastButtonDebouncer = Bounce();
int speed = 1000;
unsigned long lastTime = millis();

void setup() {
  pinMode(SLOW_BUTTON, INPUT_PULLUP);
  slowButtonDebouncer.attach(SLOW_BUTTON);
  slowButtonDebouncer.interval(10);
  
  pinMode(FAST_BUTTON, INPUT_PULLUP);
  fastButtonDebouncer.attach(FAST_BUTTON);
  fastButtonDebouncer.interval(10);
  
  pinMode(DIR, OUTPUT);
  digitalWrite(DIR, speed > 0);
  pinMode(STEP, OUTPUT);
  digitalWrite(STEP, 0);
  
  Serial.begin(9600);
}

void loop() {
  if (speed != 0) {
    int absSpeed = abs(speed) - 1;
    unsigned long time = millis();
    if (time - lastTime > 1000 / absSpeed) {
      digitalWrite(STEP, 1);
      digitalWrite(STEP, 0);
      lastTime = time;
    }
  }
  if (slowButtonDebouncer.update()) {
    if (!slowButtonDebouncer.read()) {
      speed--;
      digitalWrite(DIR, speed > 0);
      Serial.println(speed);
    }
  }
  if (fastButtonDebouncer.update()) {
    if (!fastButtonDebouncer.read()) {
      speed++;
      digitalWrite(DIR, speed > 0);
      Serial.println(speed);
    }
  }
}
