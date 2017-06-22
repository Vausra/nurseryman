/* Connection pins:
Arduino     Soil Moisture Sensor YL-69
  A0             Analog A0
  5V               VCC
  GND              GND
*/

void setup()
{
  Serial.begin(9600);
 /* pinMode(A0, INPUT); //set up analog pin 0 to be input
  pinMode(2, OUTPUT); // red led
  pinMode(3, OUTPUT); // yellow led
  pinMode(4, OUTPUT); // green led*/
}

void loop()
{
  int s = analogRead(A0); //take a sample
  Serial.print(s); Serial.print(" - ");
  
  if(s >= 1000) {
   Serial.println("Sensor is not in the Soil or DISCONNECTED");
   /*digitalWrite(2, LOW);
   digitalWrite(3, LOW);
   digitalWrite(4, LOW); */
  }
  if(s < 1000 && s >= 600) { 
   Serial.println("Soil is DRY");
   /*digitalWrite(2, LOW);
   digitalWrite(3, LOW);
   digitalWrite(4, HIGH); */
  }
  if(s < 600 && s >= 370) {
   Serial.println("Soil is HUMID"); 
   /*digitalWrite(2, LOW);
   digitalWrite(3, HIGH);
   digitalWrite(4, LOW);*/
  }
  if(s < 370) {
   Serial.println("Sensor in WATER");
   /*digitalWrite(2, HIGH);
   digitalWrite(3, LOW);
   digitalWrite(4, LOW);*/
  }
  delay(200);
}
