// Relais Steuerpin an Arduino 8
const int relaisPin = 3;
 
void setup() {
  pinMode(relaisPin, OUTPUT);
}
 
void loop() {
  digitalWrite(relaisPin, HIGH);
  delay(500);
  digitalWrite(relaisPin, LOW);
  delay(1000);
}
