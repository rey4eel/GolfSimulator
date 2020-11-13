// Include the AccelStepper library:
#include <AccelStepper.h>

// Motor pin definitions:
#define MOTOR_A_ENABLE_PIN 8
#define MOTOR_A_STEP_PIN 2
#define MOTOR_A_DIR_PIN 5
#define MOTOR_B_ENABLE_PIN 8
#define MOTOR_B_STEP_PIN 3
#define MOTOR_B_DIR_PIN 6
#define MotorInterfaceType 8
#define MOTOR_C_ENABLE_PIN 8
#define MOTOR_C_STEP_PIN 4
#define MOTOR_C_DIR_PIN 7
#define MotorInterfaceType 8

// Other PIN + signal
#define home_switch_1 11 // Pin 11 connected to Home Switch (EndSwitch)
#define home_switch_2 10 // Pin 10, connected to Home Switch (EndSwitch)
#define home_switch_3 9 // Pin 9 connected to Home Switch (EndSwitch)


//Sensor defenition
const int middleLiftSensor = 12; // Middle lift sensor
const int shootingSensor = 17; // Shooting sensor
const int feederSensor = 13; // Feeder Sensor

// Ball sensors data
// typedef enum
// {
//     TOP = -15750,
//     MIDDLE = -1000,
//     BOTTOM = 500
// } liftHight;

//  liftHight ballPosition;


AccelStepper stepperX_1 = AccelStepper(1, MOTOR_A_STEP_PIN, MOTOR_A_DIR_PIN);
AccelStepper stepperX_2 = AccelStepper(1, MOTOR_B_STEP_PIN, MOTOR_B_DIR_PIN);
AccelStepper stepperX_3 = AccelStepper(1, MOTOR_C_STEP_PIN, MOTOR_C_DIR_PIN);

// Stepper Travel Variables
long initial_homing_1 = -1; // Used to Home Stepper at startup
long initial_homing_2 = -1; // Used to Home Stepper at startup
long initial_homing_3 = -1; // Used to Home Stepper at startup

//Shooting lift position var and const
long shootingLiftPosition = -15750;
const int switchLow = 14; // rotary switch 14
const int switchMidlle= 15; // rotary switch 15
const int switchHight = 16; // rotary switch 16
const int pauseButton = 19; // Pause switch 16
volatile boolean shooted = false;

void setup() {
// Do homming if status true
bool hommingStatus = DoHomomming();

}
void loop()
{
  //Check switch Position
  CheckLiftHight();
  Pause();

  // Preposition loading waiting for the entry sensor
  if (digitalRead(feederSensor) == 0)
  {
    delay(500);
    stepperX_3.moveTo(-001); // ball roll back -->feeder
    stepperX_3.runToPosition();
    delay(500);
    stepperX_3.moveTo(-12000);// ball shoul till and roll -->feeder
    stepperX_3.runToPosition();
  }

  // this is possition for the first lift
  if (digitalRead(middleLiftSensor) == 0 && shooted == true) {
    delay(500);
    stepperX_2.moveTo(-10500); // --> MidlleLift going UP
    stepperX_2.runToPosition();
  }


  // This is possition for the ball shoot position
  if (digitalRead(shootingSensor) == 0) {
      delay(500);
      // Move Midle lift back -> unblock feeder
      stepperX_2.moveTo(-400);
      stepperX_2.runToPosition();
      // Ready for Feeding only if the middle is ready to take
      if(digitalRead(middleLiftSensor) == 1)
      {
        // Open feeder - >
        stepperX_3.moveTo(-2500);
        stepperX_3.runToPosition();
      }
      // Go Up for shooting possition
      stepperX_1.moveTo(shootingLiftPosition);
      stepperX_1.runToPosition();
      shooted = false;
      delay(500);
  }

  if (digitalRead(shootingSensor) == 1)
  {
    delay(500);
    // Move shooting lift down when ball shooted
    stepperX_1.moveTo(-001);
    stepperX_1.runToPosition();
    delay(100);
    shooted = true;
  }

}
bool DoHomomming()
{
  Serial.begin(9600);
  pinMode(initial_homing_1, INPUT_PULLUP);
  pinMode(initial_homing_2, INPUT_PULLUP);
  pinMode(initial_homing_3, INPUT_PULLUP);
  pinMode(MOTOR_A_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_B_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_C_ENABLE_PIN, OUTPUT);
  pinMode(middleLiftSensor, INPUT);
  pinMode(shootingSensor, INPUT);
  pinMode(feederSensor, INPUT);
  pinMode(switchLow, INPUT);
  pinMode(switchMidlle, INPUT);
  pinMode(switchHight, INPUT);
  pinMode(pauseButton, INPUT);

  delay(5);  // Wait for driver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  stepperX_1.setMaxSpeed(9999.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperX_1.setAcceleration(3500.0);  // Set Acceleration of Stepper

  stepperX_2.setMaxSpeed(7777.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperX_2.setAcceleration(2500);  // Set Acceleration of Stepper


  stepperX_3.setMaxSpeed(7777.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperX_3.setAcceleration(3500.0);  // Set Acceleration of Stepper


  while (digitalRead(home_switch_1)) {  // Make the Stepper move CCW until the switch is activated
    Serial.println("Homing shooting lift Start");
    stepperX_1.moveTo(initial_homing_1);  // Set the position to move to
    initial_homing_1--;  // Decrease by 1 for next move if needed
    stepperX_1.run();  // Start moving the stepper
    delay(5);
  }

  stepperX_1.setCurrentPosition(0);  // Set the current position as zero for now
  stepperX_1.setMaxSpeed(7777.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperX_1.setAcceleration (5500.0);  // Set Acceleration of Stepper
  initial_homing_1 = 1;

  while (!digitalRead(home_switch_1)) { // Make the Stepper move CW until the switch is deactivated
    stepperX_1.moveTo(initial_homing_1);
    stepperX_1.run();
    initial_homing_1++;
    delay(5);
  }

  stepperX_1.setCurrentPosition(0);
  stepperX_1.setMaxSpeed(8888);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperX_1.setAcceleration(5500.0);  // Set Acceleration of Stepper

  while (digitalRead(home_switch_2)) { // Make the Stepper move CCW until the switch is activated
    Serial.println("Homing Midlle lift Start");
    stepperX_2.moveTo(initial_homing_2);
    initial_homing_2--;
    stepperX_2.run();
    delay(5);
  }

  stepperX_2.setCurrentPosition(0);  // Set the current position as zero for now
  stepperX_2.setMaxSpeed(8888.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperX_2.setAcceleration(3500.0);  // Set Acceleration of Stepper
  initial_homing_2 = 1;


  while (!digitalRead(home_switch_2)) { // Make the Stepper move CCW until the switch is activated
    stepperX_2.moveTo(initial_homing_2);
    initial_homing_2++;
    stepperX_2.run();
    delay(5);
  }
  Serial.println("Homing Midlle lift Finished");

  stepperX_2.setCurrentPosition(0);
  stepperX_2.setMaxSpeed(8888.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperX_2.setAcceleration(4500.0);  // Set Acceleration of Stepper


  while (digitalRead(home_switch_3)) {  // Make the Stepper move CCW until the switch is activated
    Serial.println("Homing Feeder lift Start");
    stepperX_3.moveTo(initial_homing_3);  // Set the position to move to
    initial_homing_3--;  // Decrease by 1 for next move if needed
    stepperX_3.run();  // Start moving the stepper
    delay(5);
  }


  stepperX_3.setCurrentPosition(0);  // Set the current position as zero for now
  stepperX_3.setMaxSpeed(8888.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperX_3.setAcceleration(4500.0);  // Set Acceleration of Stepper
  initial_homing_3 = 1;

  while (!digitalRead(home_switch_3)) { // Make the Stepper move CW until the switch is deactivated
    stepperX_3.moveTo(initial_homing_3);
    stepperX_3.run();
    initial_homing_3++;
    delay(5);
  }
  Serial.println("Homing Feeder lift Finished");

  stepperX_3.setCurrentPosition(0);
  stepperX_3.setMaxSpeed(8888.0);      // Set Max Speed of Stepper (Faster for regular movements) was 6555 down to 4500
  stepperX_3.setAcceleration(4500.0);  // Set Acceleration of Stepper was 35000 down to 1500

  // Move the lifts to some position higher than the LimtSensor
  stepperX_1.moveTo(-1);  // Can be any value
  stepperX_1.runToPosition();
  stepperX_2.moveTo(-400);  // Can be any value
  stepperX_2.runToPosition();
  stepperX_3.moveTo(-2500);  // blocked position for prepostion lift
  stepperX_3.runToPosition();

  Serial.println ("------------------------------------------------------------------------------------------------");
  return true;
}
void UnloadFromMidlleLift()
{
    stepperX_3.moveTo(-11000);
    stepperX_3.runToPosition();
    delay(500);
    stepperX_2.moveTo(-10500);
    stepperX_2.runToPosition();
}
void UnloadFromFeeder()
{
    delay(500);
    stepperX_3.moveTo(-001); // ball roll back
    stepperX_3.runToPosition();
    delay(500);
    stepperX_3.moveTo(-12000);// ball shoul till and roll
    stepperX_3.runToPosition();
    stepperX_3.moveTo(-12000);// Ball Block position up
    stepperX_3.runToPosition();
}
void UnloadFromShootingLift()
{
      delay(500);
      stepperX_2.moveTo(-400);
      stepperX_2.runToPosition();
      stepperX_1.moveTo(-15750 );
      stepperX_1.runToPosition();
      delay(500);
}
void CheckLiftHight()
{
  if(digitalRead(switchLow) == 0)
  {
    shootingLiftPosition = -10000;
  }
  if(digitalRead(switchMidlle) == 0)
  {
    shootingLiftPosition = -10000;
  }
  if(digitalRead(switchHight) == 0)
  {
    shootingLiftPosition = -15750;
  }
}
void Pause()
{
    // Wait until resetbutton becomes HIGH
    // while (digitalRead(pauseButton) == 1) {
    //   delay(1);
    // }
}
