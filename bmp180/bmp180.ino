
/* 

Any Arduino pins labeled:  SDA  SCL
Uno                         A4   A5
*/

#include <SFE_BMP180.h>
#include <Wire.h>

SFE_BMP180 pressure;

double baseline; // baseline pressure

// YL-39 + YL-69 humidity sensor
//byte humidity_sensor_pin = A0;
//byte humidity_sensor_vcc = 4;

void setup()
{
  //pinMode(humidity_sensor_vcc, OUTPUT);
  //pinMode(humidity_sensor_pin, INPUT);
  //digitalWrite(humidity_sensor_vcc, LOW);
  
  
  while (!Serial);
  delay(1000);
  Serial.begin(9600);

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }

 
}

int read_humidity_sensor() {
  //digitalWrite(humidity_sensor_vcc, HIGH);
  //delay(500);
  //int value = analogRead(humidity_sensor_pin);
  //digitalWrite(humidity_sensor_vcc, LOW);
  //return 1023 - value;
}


void loop()
{
  double a,P;
  
  P = getPressure();

  // Show the relative difference between
  //  new and the baseline (old) reading
  a = pressure.altitude(P,baseline);
  
  /*Serial.print("relative altitude: ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  Serial.print(a,1);
  Serial.print(" meters, ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
*/
  Serial.print("pressure: ");
  Serial.print(P);
  Serial.print("\n");
  
  //Serial.print("Humidity Level (0-1023): ");
  //Serial.println(read_humidity_sensor()); 
  delay(2000);
}


double getPressure()
{
  char status;
  double T,P,p0,a;

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}

