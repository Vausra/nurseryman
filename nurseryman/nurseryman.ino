
#include <Wire.h>
#include <Stepper.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"
#include <SFE_BMP180.h>

/*****************init pins*************************/
#define DHTPIN  4         //Digital pin 4
#define DHTTYPE DHT22
#define SDCARD  13        //Digital pin 4
#define RELAY  3          //Digital pin 3
#define END_SWITCH 2      //Digital pin 2
#define YL_69_D 5           //Digital pin 

/*Analog Pins*/
#define YL_69_A 1
/***************************************************/

/*****************const variables*******************/
const int stepsPerRevolution = 200; //each step is 1.8 degree. 200 means full rotatoin

const int upper_destination = 6900;//345mm upper_limit * steps for a full rotation (200 steps), /10 due to step motor does 10 steps
const int lower_destination = 700;//35mm lower_limit * steps for a full rotation (200 steps), /10 due to step motor does 10 steps
const int upper_limit = 76000;//380mm whole distance until end_switch is reached
const int lower_limit = 0;//380mm whole distance until end_switch is reached
/***************************************************/

/*****************init libraries********************/
//360 degree ( a full rotation means about 8mm distance on x-axis
//from side to side (end switch to end switch distance is about 38 cm
//Between the plats are 31cm on each side. 3,5 cm after plant end switch is reached.
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 barometer;
/***************************************************/

/*****************variables*************************/
int count;  //count how much steps were made. One step means 1.8 degree.
int current_position; //on reboot arduino has no clue where step motor is.
int irrigation_need = 0; //used as boolean. 1 run irrigation, 0 not.
int button_state = 0; //used to determine if switch is pressed or not

//int wait_15_min = 900000; //wait 15 min until next measurement
int wait_10_sec = 10000; //wait 10 sec to water plants

char status;
/***************************************************/

/*****************sensor variables ******************/
float temperature_DHT_22_value;
float humidity_value_air;
int humidity_value_ground;
double pressure_bmp180;
double temperature_bmp180;

/***************************************************/

int humidity_ground_func() {

  humidity_value_ground = analogRead(A3);

  if (humidity_value_ground >= 1000) {
    Serial.println("Sensor is not in the Soil or DISCONNECTED");
    return 0;
  }
  if (humidity_value_ground < 1000 && humidity_value_ground >= 600) {
    Serial.println("Soil is DRY, watering!");
    return 1;

  }
  if (humidity_value_ground < 600 && humidity_value_ground >= 370) {
    Serial.println("Soil is HUMID, no need for watering!");
    return 0;

  }
  if (humidity_value_ground < 370) {
    Serial.println("Sensor in WATER, check for overflow!");
    return 0;

  }
  return 0;
}

void read_dht_22() {

  temperature_DHT_22_value = dht.readTemperature();
  humidity_value_air = dht.readHumidity();

}

void read_bmp180(double pressure, double temperature) {
  status = barometer.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = barometer.getTemperature(temperature);
    if (status != 0)
    {
      status = barometer.startPressure(3); //3 means highest resolution
      if (status != 0)
      {
        delay(status);

        status = barometer.getPressure(pressure, temperature);
        if (status != 0)

        {

        }
       
      }
      
    }
    
  }

}

int write_to_sdcard(String data_string) {
  File data_file = SD.open("datalog.txt", FILE_WRITE);

  if (data_file) {
    data_file.println(data_string);
    data_file.close();
    // print to the serial port too:
    Serial.println(data_string);
    return 1;
  }
  else {
    Serial.println("error opening datalog.txt");
    return 0; //bad things happened;
  }

}


String float_to_string(float value)
{
  return String(value);
}

String int_to_string(int value)
{
  return String(value);
}

String long_to_string(long value) {
  return String(value);
}

String double_to_string(double value) {
  return String(value);
}

void setup() {
  //initialize the serial port:
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  if (!SD.begin(SDCARD)) {
    Serial.println("Card failed, or not present");
    while (1);
  }

  dht.begin();

  Wire.begin();


  if (!barometer.begin()) {
    while (1);
  }

  myStepper.setSpeed(100); //200 is max speed
  pinMode(end_switch, INPUT);
  pinMode(A3, INPUT); //Analog pin A3 for YL-69
  button_state = 0;
}

void loop() {


  count = 0;

  if (humidity_ground_func()) {

    //assume that stepmotor is init in correct minimum position!!!!
    //probably runs one time to get max and min and in next iteration irrigation
    //step motor always start with clockwise rotation
    //run as long as destination is reached or end_switch.
    while (1 || count != upper_destination)
    {
      button_state = digitalRead(end_switch);
      if (button_state == LOW) //HIGH means button is pressed (pull down circuit) !
      {
        myStepper.step(10); //step motor run with clockwise rotation. Do 10 steps.
        count++;
      }
      else
      {
        current_position = upper_limit;
        break;
      }
    }
    if (count == upper_destination) {
      //Begin pump. No method here because this is too low level.
      digitalWrite(RELAIS, HIGH); //Turn pump on;
      delay(10000); //wait 10 sec;
      digitalWrite(RELAIS, LOW); //Turn pump off;
      //end pump
    }
  }

  count = 0;

  while (1 || count != lower_destination)
  {
    button_state = digitalRead(end_switch);
    if (button_state == LOW) //HIGH means button is pressed (pull down circuit) !
    {
      myStepper.step(-10); //step motor run anti-clockwise rotation. Do 10 steps.
      count++;
    }
    else
    {
      current_position = lower_limit;
      break;
    }

    if (count == lower_destination) {
      //Begin pump. No method here because this is too low level.
      digitalWrite(RELAIS, HIGH); //Turn pump on;
      delay(10000); //wait 10 sec;
      digitalWrite(RELAIS, LOW); //Turn pump off;
      //end pump
    }
  } //end of moving phase.

  //start read from barometric preasure sensor
  read_bmp180(pressure_bmp180, temperature_bmp180);

  //store all values
  write_to_sdcard(double_to_string(pressure_bmp180));
  write_to_sdcard(double_to_string(temperature_bmp180));
  write_to_sdcard(int_to_string(humidity_value_ground));
  write_to_sdcard(float_to_string(temperature_DHT_22_value));
  write_to_sdcard(float_to_string(humidity_value_air));

  write_to_sdcard("======end=====");


  //wait 15 min
  for (int i = 0; i < 100; i++) {
    delay(9000); //wait 15 min
  }
}






