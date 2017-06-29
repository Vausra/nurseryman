/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

   SD card attached to SPI bus as follows:
   MOSI - pin 11
   MISO - pin 12
   CLK - pin 13
   CS - pin 10
   SDA    A4
   SCL    A5
*/

#include <Stepper.h>
#include <SPI.h>
#include <SD.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include "DHT.h" //DHT Bibliothek laden

#define SD_CARD 10
#define DHTPIN 2 //Der Sensor wird an PIN 2 angeschlossen    
#define DHTTYPE DHT22    // Es handelt sich um den DHT22 Sensor

/*****************stepper*******************/
const int stepsPerRevolution = 200;
Stepper my_stepper(stepsPerRevolution, 6, 7, 8, 9);

const int upper_destination = 750;//345mm upper_limit * steps for a full rotation (200 steps), /10 due to step motor does 10 steps
const int lower_destination = 70;//35mm lower_limit * steps for a full rotation
const int upper_limit = 780;//380mm whole distance until end_switch is reached
const int lower_limit = 0;//380mm whole distance until end_switch is reached

int current_position; //on reboot arduino has no clue where step motor is.
int count;  //count how much steps were made. One step means 1.8 degree.
/***************************************************/

const byte end_switch = 3;
const byte relay_stepper = 5;
const byte relay_pump = 4;
byte button_state = 0;


DHT dht(DHTPIN, DHTTYPE); //Der Sensor wird ab jetzt mit „dth“ angesprochen

SFE_BMP180 pressure;

// YL-39 + YL-69 humidity sensor
byte soil_humidity_sensor_pin = A0;

//switch off stepper turn on pump and backwards
void irrigate() {
  delay(500);
  digitalWrite(relay_pump, LOW);
  Serial.print("Pump on!\n");
  delay(10000);
  digitalWrite(relay_pump, HIGH);
  delay(500);
}

void write_to_log(String log_msg)
{
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(log_msg);
    dataFile.close();
    // print to the serial port too:
    Serial.println(log_msg);
  }
  else
  {
    Serial.println("sd error");
  }
}

int get_ground_humidity() {
   return 1023 - analogRead(soil_humidity_sensor_pin);
}

double getPressure()
{
  char status;
  double T, P, p0, a;

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

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          return (P);
        }
      }
    }
  }
}

void move_stepper()
{
  button_state = digitalRead(end_switch);

  digitalWrite(relay_stepper, LOW);
  //drive to upper
 
  while (1) {
    button_state = digitalRead(end_switch);
    if(button_state == HIGH) {
      break;
    }
    my_stepper.step(10);
    count++;
       
    if (count == upper_destination) {
      Serial.print("upper destination reached\n");
      button_state = LOW;
      
      digitalWrite(relay_stepper, HIGH); //off
      irrigate();
      digitalWrite(relay_stepper, LOW); //on
      break;
    }
  }

  if (button_state == HIGH) {
    count = upper_limit;
    Serial.print("Upper limit");
    //drive in opposite direction, to get off the endswitch
    for (int i = 0; i < 30; i++)
    {
      my_stepper.step(-10);
      count--;
    }
  }
  button_state = LOW;

  //drive to lower limit
  while (1) {
    button_state = digitalRead(end_switch);
    
    if(button_state == HIGH) {
      break;
    }
    
    my_stepper.step(-10);
    count--;
    
    if (count == lower_destination) {
      Serial.print("lower destination reached\n");
      button_state = LOW;
      digitalWrite(relay_stepper, HIGH); //off
      irrigate();
      digitalWrite(relay_stepper, LOW); //on
      break;
      
    }
  }

  if (button_state == HIGH) {
    count = lower_limit;
    
    //drive in opposite direction, to get off the endswitch
    for (int i = 0; i < 30; i++)
    {
      my_stepper.step(+10);
      count++;
    }
  }
  button_state = LOW;

  digitalWrite(relay_stepper, HIGH);
  delay(10000);

}

void setup() {

  pinMode(soil_humidity_sensor_pin, INPUT);

  while (!Serial);
  delay(1000);
  Serial.begin(9600);

  /*if (!pressure.begin())
  {
    Serial.println("BMP180 init fail");
    while(1);
  }*/

  Serial.println("BMP180 init success");

  dht.begin(); //DHT22 Sensor starten

  if (!SD.begin(SD_CARD)) {
    Serial.println("SD init fail");

    // don't do anything more:
    while(1);
  }

  pinMode(end_switch, INPUT);

  pinMode(relay_stepper, OUTPUT);
  digitalWrite(relay_stepper, LOW);

  pinMode(relay_pump, OUTPUT);
  digitalWrite(relay_pump, HIGH);

  my_stepper.setSpeed(200);

}

void loop() {


  String data_string = "";
  String pump_string = "no pump";

  float humidity_soil = get_ground_humidity();

  if (humidity_soil < 620)
  {

    pump_string = "pump run";
    move_stepper();

  }


  float air_humidity = dht.readHumidity(); //die Luftfeuchtigkeit auslesen und unter „Luftfeutchtigkeit“ speichern
  data_string = air_humidity;

  data_string += " %, ";
  float temperature = dht.readTemperature();//die Temperatur auslesen und unter „Temperatur“ speichern
  data_string += temperature;
  data_string += " C, ";
  //data_string += getPressure();
  //data_string += " mb, ";
  data_string += humidity_soil;
  data_string += " %, ";
  data_string += pump_string;

  write_to_log(data_string);

  delay(900000);
}




