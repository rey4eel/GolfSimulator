#include <AccelStepper.h>

#define MOTOR_A_ENABLE_PIN 8
#define MOTOR_A_STEP_PIN   2
#define MOTOR_A_DIR_PIN    5
#define MOTOR_B_ENABLE_PIN 8
#define MOTOR_B_STEP_PIN   3
#define MOTOR_B_DIR_PIN    6
#define MOTOR_C_ENABLE_PIN 8
#define MOTOR_C_STEP_PIN   4
#define MOTOR_C_DIR_PIN    7
#define home_switch_1      11  
#define home_switch_2      10  
#define home_switch_3      9  
#define DEBUG              1

#if DEBUG == 1
  #define debug(x)   Serial.print(x);
  #define debugln(x) Serial.println(x);
#else
  #define debug(x);
  #define debugln(x);
#endif

enum MotorPosotion : long {
  feederLoading = -1,
  feederDeliver = -3600,
  middleLoading = -400,
  middleDeliver = -11200,
  teeLoading =    -1,
};

byte feederPos = 0, middlePos = 0, teePos = 0;
const int middleLiftSensor =           12;    
const int shootingSensor =             17;      
const int feederSensor =               13;        
int tee_homing_sensor =                -1;          
int middle_homing_sensor =             -1;          
int feeder_homing_sensor =             -1;          
volatile int shootingLiftPosition =    -1000;

AccelStepper teeMotor    = AccelStepper(1, MOTOR_A_STEP_PIN, MOTOR_A_DIR_PIN);
AccelStepper middleMotor = AccelStepper(1, MOTOR_B_STEP_PIN, MOTOR_B_DIR_PIN);
AccelStepper feederMotor = AccelStepper(1, MOTOR_C_STEP_PIN, MOTOR_C_DIR_PIN);

bool isBallOnSensor(int sensor, bool converted = true) {
  if (!converted) return digitalRead(sensor);
  return !digitalRead(sensor);
}

void setup() {
  Serial.begin(9600);
  tryHomomming();
}

void loop() {
  
  if (!isBallOnSensor(feederSensor)) {
    motorMove(feederMotor, MotorPosotion::feederLoading);
  }

  if (isBallOnSensor(feederSensor) && !isBallOnSensor(middleLiftSensor) && middlePos == 0) {
    motorMove(feederMotor, MotorPosotion::feederDeliver);
  }

  if (!isBallOnSensor(middleLiftSensor)) {
    motorMove(middleMotor, MotorPosotion::middleLoading);
    middlePos = 0;
  }

  if (isBallOnSensor(middleLiftSensor) && !isBallOnSensor(shootingSensor, false) && teePos == 0) {
    motorMove(middleMotor, MotorPosotion::middleDeliver);
    middlePos = 1;
  }

  if (isBallOnSensor(shootingSensor, false)) {
    motorMove(teeMotor,-3000);
    teePos = 1;
  }

  if (!isBallOnSensor(shootingSensor, false)) {
    motorMove(teeMotor, MotorPosotion::teeLoading);
    teePos = 0;
  }
}

void tryHomomming() {
  pinMode(tee_homing_sensor, INPUT_PULLUP);
  pinMode(middle_homing_sensor, INPUT_PULLUP);
  pinMode(feeder_homing_sensor, INPUT_PULLUP);
  pinMode(MOTOR_A_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_B_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_C_ENABLE_PIN, OUTPUT);
  pinMode(middleLiftSensor, INPUT);
  pinMode(shootingSensor, INPUT);
  pinMode(feederSensor, INPUT);

  delay(5);  // Wait for driver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  teeMotor.setMaxSpeed(9999.0);             // Set Max Speed of Stepper (Slower to get better accuracy)
  teeMotor.setAcceleration(3500.0);         // Set Acceleration of Stepper

  middleMotor.setMaxSpeed(7777.0);          // Set Max Speed of Stepper (Slower to get better accuracy)
  middleMotor.setAcceleration(2500);        // Set Acceleration of Stepper

  feederMotor.setMaxSpeed(7777.0);          // Set Max Speed of Stepper (Slower to get better accuracy)
  feederMotor.setAcceleration(3500.0);      // Set Acceleration of Stepper

  while (digitalRead(home_switch_1)) {      // Make the Stepper move CCW until the switch is activated
    teeMotor.moveTo(tee_homing_sensor);      // Set the position to move to
    tee_homing_sensor--;                     // Decrease by 1 for next move if needed
    teeMotor.run();                         // Start moving the stepper
    delay(5);
  }

  teeMotor.setCurrentPosition(0);           // Set the current position as zero for now
  teeMotor.setMaxSpeed(7777.0);             // Set Max Speed of Stepper (Slower to get better accuracy)
  teeMotor.setAcceleration(5500.0);         // Set Acceleration of Stepper
  tee_homing_sensor = 1;

  while (!digitalRead(home_switch_1)) {     // Make the Stepper move CW until the switch is deactivated
    teeMotor.moveTo(tee_homing_sensor);
    teeMotor.run();
    tee_homing_sensor++;
    delay(5);
  }

  teeMotor.setCurrentPosition(0);
  teeMotor.setMaxSpeed(8888);               // Set Max Speed of Stepper (Faster for regular movements)
  teeMotor.setAcceleration(5500.0);         // Set Acceleration of Stepper

  while (digitalRead(home_switch_2)) {      // Make the Stepper move CCW until the switch is activated
    middleMotor.moveTo(middle_homing_sensor);
    middle_homing_sensor--;
    middleMotor.run();
    delay(5);
  }

  middleMotor.setCurrentPosition(0);        // Set the current position as zero for now
  middleMotor.setMaxSpeed(8888.0);          // Set Max Speed of Stepper (Slower to get better accuracy)
  middleMotor.setAcceleration(3500.0);      // Set Acceleration of Stepper
  middle_homing_sensor = 1;

  while (!digitalRead(home_switch_2)) {     // Make the Stepper move CCW until the switch is activated
    middleMotor.moveTo(middle_homing_sensor);
    middle_homing_sensor++;
    middleMotor.run();
    delay(5);
  }

  middleMotor.setCurrentPosition(0);
  middleMotor.setMaxSpeed(8888.0);          // Set Max Speed of Stepper (Faster for regular movements)
  middleMotor.setAcceleration(4500.0);      // Set Acceleration of Stepper

  while (digitalRead(home_switch_3)) {      // Make the Stepper move CCW until the switch is activated
    feederMotor.moveTo(feeder_homing_sensor);   // Set the position to move to
    feeder_homing_sensor--;                     // Decrease by 1 for next move if needed
    feederMotor.run();                      // Start moving the stepper
    delay(5);
  }

  feederMotor.setCurrentPosition(0);        // Set the current position as zero for now
  feederMotor.setMaxSpeed(8888.0);          // Set Max Speed of Stepper (Slower to get better accuracy)
  feederMotor.setAcceleration(4500.0);      // Set Acceleration of Stepper
  feeder_homing_sensor = 1;

  while (!digitalRead(home_switch_3)) {     // Make the Stepper move CW until the switch is deactivated
    feederMotor.moveTo(feeder_homing_sensor);
    feederMotor.run();
    feeder_homing_sensor++;
    delay(5);
  }

  feederMotor.setCurrentPosition(0);
  feederMotor.setMaxSpeed(4444.0);          // Set Max Speed of Stepper (Faster for regular movements) was 6555 down to 4500
  feederMotor.setAcceleration(2222.0);      // Set Acceleration of Stepper was 35000 down to 1500

  // Move the lifts to some position higher than the LimtSensor
  motorMove(teeMotor, MotorPosotion::teeLoading);
  motorMove(middleMotor, MotorPosotion::middleLoading);
  motorMove(feederMotor, MotorPosotion::feederLoading);

}

void motorMove(AccelStepper &motor, int value) {
    motor.moveTo(value);
    motor.runToPosition();
}
