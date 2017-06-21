//Pull down 100k Ohm zwischen plus und digiPin

const int buttonPin = 2;
int buttonState = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.print("High");
  }
  else {
    Serial.print("Low");
  }
}

