#include <Stepper.h>


/*****************stepper*******************/
const int stepsPerRevolution = 200;
Stepper my_stepper(stepsPerRevolution, 6, 7, 8, 9);

const int upper_destination = 790;//345mm upper_limit * steps for a full rotation (200 steps), /10 due to step motor does 10 steps
const int lower_destination = 50;//35mm lower_limit * steps for a full rotation
const int upper_limit = 760;//380mm whole distance until end_switch is reached
const int lower_limit = 0;//380mm whole distance until end_switch is reached

int current_position; //on reboot arduino has no clue where step motor is.
int count;  //count how much steps were made. One step means 1.8 degree.
/***************************************************/

const byte end_switch = 3;
const byte relay_stepper = 5;
const byte relay_pump = 4;
byte button_state = 0;

void irrigate() {
  delay(1000);
  
  digitalWrite(relay_pump, LOW);
  Serial.print("Pump on!\n");
  delay(2000);
  digitalWrite(relay_pump, HIGH);
  
  delay(1000);
  
}

void setup() {
  pinMode(end_switch, INPUT);
  
  pinMode(relay_stepper, OUTPUT);
  digitalWrite(relay_stepper, HIGH);

  pinMode(relay_pump, OUTPUT);
  digitalWrite(relay_pump, HIGH);

  Serial.begin(9600);
  my_stepper.setSpeed(200);

  count = 70;
}

void loop() {

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

  Serial.print(count);
  Serial.print("\n");
  
  Serial.print("Button State: ");
  Serial.print(button_state);
  Serial.print("\n");

  delay(1000);

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
  
  delay(1000);

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

  Serial.print(count);
  Serial.print("\n");

  Serial.print("Button State: ");
  Serial.print(button_state);
  Serial.print("\n");

  delay(1000);

  if (button_state == HIGH) {
    count = lower_limit;
    Serial.print("lower limit");
    //drive in opposite direction, to get off the endswitch
    for (int i = 0; i < 70; i++)
    {
      my_stepper.step(+10);
      count++;
    }
  }
  
  digitalWrite(relay_stepper, HIGH);
  delay(10000);

}

