#include <Stepper.h>

const int stepsPerRevolution = 200;

const byte relayPin = A1;
Stepper my_stepper(stepsPerRevolution, 6, 7, 8, 9);

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  Serial.begin(9600);
  my_stepper.setSpeed(200);
}

void loop() {
      digitalWrite(relayPin, LOW); //Turn pump on;
      delay(1000); //wait 10 sec;
      my_stepper.step(-100);
      delay(1000); //wait 10 sec;
      digitalWrite(relayPin, HIGH); //Turn pump off;
      
      delay(10000); //wait 10 sec;
}
