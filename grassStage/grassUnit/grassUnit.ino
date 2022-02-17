#include <AccelStepper.h>

#define MOTOR_ENABLE_PIN   8
#define MOTOR_A_STEP_PIN   2
#define MOTOR_A_DIR_PIN    5
#define MOTOR_B_STEP_PIN   3
#define MOTOR_B_DIR_PIN    6
#define MOTOR_C_STEP_PIN   4
#define MOTOR_C_DIR_PIN    7
#define home_switch_tee    11  
#define home_switch_middle 10  
#define home_switch_feeder 9  
#define middleLiftSensor   12  
#define feederSensor       13  
#define shootingSensor     17 
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
  middleDeliver = -11500,
  teeLoading =    -1,
};

byte feederPos = 0, middlePos = 0, teePos = 0;        
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
    moveMotor(feederMotor, MotorPosotion::feederLoading);
  }

  if (isBallOnSensor(feederSensor) && !isBallOnSensor(middleLiftSensor) && middlePos == 0) {
    moveMotor(feederMotor, MotorPosotion::feederDeliver);
  }

  if (!isBallOnSensor(middleLiftSensor)) {
    moveMotor(middleMotor, MotorPosotion::middleLoading);
    middlePos = 0;
  }

  if (isBallOnSensor(middleLiftSensor) && !isBallOnSensor(shootingSensor, false) && teePos == 0) {
    moveMotor(middleMotor, MotorPosotion::middleDeliver);
    middlePos = 1;
  }

  if (isBallOnSensor(shootingSensor, false)) {
    Serial.println(isBallOnSensor(shootingSensor, false));
    moveMotor(teeMotor,-3000);
    teePos = 1;
  }

  if (!isBallOnSensor(shootingSensor, false)) {
    Serial.println(isBallOnSensor(shootingSensor, false));
    moveMotor(teeMotor, MotorPosotion::teeLoading);
    teePos = 0;
  }
}

void tryHomomming() {
  pinMode(home_switch_tee, INPUT);
  pinMode(home_switch_middle, INPUT);
  pinMode(home_switch_feeder, INPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(middleLiftSensor, INPUT);
  pinMode(shootingSensor, INPUT);
  pinMode(feederSensor, INPUT);

  delay(5);  // Wait for driver wake up
   
  initializeMotor(teeMotor, home_switch_tee, 9999, 5500 , 9999, 5500);
  initializeMotor(middleMotor, home_switch_middle, 7777, 2500, 7777, 3500);
  initializeMotor(feederMotor, home_switch_feeder, 7777, 2500, 7777, 3500);

  // Move the lifts to some position higher than the LimtSensor
  moveMotor(teeMotor, MotorPosotion::teeLoading);
  moveMotor(middleMotor, MotorPosotion::middleLoading);
  moveMotor(feederMotor, MotorPosotion::feederLoading);

}

void initializeMotor(AccelStepper &motor, uint8_t endSwitch, int maxSpeedInit, int accelerationInit, int maxSpeedRun, int accelerationRun) {
  int position = -1;
    //  Set Max Speed and Acceleration of each Steppers at startup for homing
  motor.setMaxSpeed(maxSpeedInit);             
  motor.setAcceleration(accelerationInit);

    while (digitalRead(endSwitch)) {      // Make the Stepper move CCW until the switch is activated
      motor.moveTo(position);             // Set the position to move to
      position--;                         // Decrease by 1 for next move if needed
      teeMotor.run();                     // Start moving the stepper
      delay(5);
    }

  motor.setCurrentPosition(0);            // Set the current position as zero for now
  position = 1;

    while (!digitalRead(endSwitch)) {     
      motor.moveTo(position);
      motor.run();
      position++;
      delay(5);
    }

  motor.setCurrentPosition(0);

  motor.setMaxSpeed(maxSpeedRun);             
  motor.setAcceleration(accelerationRun);
}

void moveMotor(AccelStepper &motor, int value) {
    motor.moveTo(value);
    motor.runToPosition();
}
