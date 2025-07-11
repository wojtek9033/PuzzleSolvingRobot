#include <Arduino.h>
#include <AccelStepper.h>
#include <Servo.h>

#include <cmath>

#define BASE_STEP_PIN 2
#define BASE_DIR_PIN 3
#define SHOULDER_STEP_PIN 4
#define SHOULDER_DIR_PIN 5
#define ELBOW_STEP_PIN 6
#define ELBOW_DIR_PIN 7
#define WRIST_STEP_PIN 8
#define WRIST_DIR_PIN 9
#define VALVE_PIN 10
#define PUMP_PIN 11

// clockwise rotation means negative angle
AccelStepper base(AccelStepper::DRIVER, BASE_STEP_PIN, BASE_DIR_PIN);
AccelStepper shoulder(AccelStepper::DRIVER, SHOULDER_STEP_PIN, SHOULDER_DIR_PIN);
AccelStepper elbow(AccelStepper::DRIVER, ELBOW_STEP_PIN, ELBOW_DIR_PIN);
AccelStepper wrist(AccelStepper::DRIVER, WRIST_STEP_PIN, WRIST_DIR_PIN);

// Gripper
Servo valve;
Servo pump;

const double SHOULDER_MICROSTEPS = 16.0; // Number of microsteps
const double ELBOW_MICROSTEPS = 16.0;
const double BASE_MICROSTEPS = 4.0;
const double WRIST_MICROSTEPS = 2.0;
const double MOTOR_STEPS_PER_REV = 200.0; // Number of steps per revolution for the motor
const double SHOULDER_STEPS_PER_REV = MOTOR_STEPS_PER_REV * SHOULDER_MICROSTEPS; // Total steps per revolution
const double ELBOW_STEPS_PER_REV = MOTOR_STEPS_PER_REV * ELBOW_MICROSTEPS;
const double BASE_STEPS_PER_REV = MOTOR_STEPS_PER_REV * BASE_MICROSTEPS;
const double WRIST_STEPS_PER_REV = MOTOR_STEPS_PER_REV * WRIST_MICROSTEPS;

const double BASE_SCREW_PITCH = 2.0; // Pitch of the screw in mm  
const double SHOULDER_GEAR_RATIO = 4.5; // Gear ratio for the shoulder motor
const double ELBOW_GEAR_RATIO = 7.28; // Gear ratio for the elbow motor
const double WRIST_GEAR_RATIO = 3.0;

// Status of axis
bool baseIsMoving = false;
bool shoulderIsMoving = false;
bool elbowIsMoving = false;
bool wristIsMoving = false;

// For slope detection
bool prev_baseIsMoving = false;
bool prev_shoulderIsMoving = false;
bool prev_elbowIsMoving = false;
bool prev_wristIsMoving = false;

bool firstScan = true;

// Buffer for receiving a command
String serialData = "";
String baseStatus = "";
String shoulderStatus = "";
String elbowStatus = "";
String wristStatus = "";
String status = "";

void moveAxisRevolute(AccelStepper& motor, double angle, double steps_per_rev, double gear_ratio);
void moveAxisPrismatic(AccelStepper& motor, double steps_per_rev, double distance);
void parseCommand(String cmd);
void controlGripper(int state);

void setup() {

  base.setMaxSpeed(BASE_STEPS_PER_REV * 10);
  shoulder.setMaxSpeed(SHOULDER_STEPS_PER_REV * 2);
  elbow.setMaxSpeed(ELBOW_STEPS_PER_REV * 2);
  wrist.setMaxSpeed(WRIST_STEPS_PER_REV);
/*
  base.setMaxSpeed(STEPS_PER_REV * 6.0);
  shoulder.setMaxSpeed(STEPS_PER_REV  * 2.0);
  elbow.setMaxSpeed(STEPS_PER_REV * 2.0);
  wrist.setMaxSpeed(STEPS_PER_REV);
*/
  base.setAcceleration(BASE_STEPS_PER_REV * 4.0);
  shoulder.setAcceleration(SHOULDER_STEPS_PER_REV * 4.0);
  elbow.setAcceleration(ELBOW_STEPS_PER_REV * 4.0);
  wrist.setAcceleration(WRIST_STEPS_PER_REV * 2.0);

  //basing
  base.setCurrentPosition(0);
  shoulder.setCurrentPosition(0);
  elbow.setCurrentPosition(0);
  wrist.setCurrentPosition(0);

  valve.attach(VALVE_PIN);
  pump.attach(PUMP_PIN);

  Serial.begin(9600);
}

void loop() {
  prev_baseIsMoving = baseIsMoving;
  prev_shoulderIsMoving = shoulderIsMoving;
  prev_elbowIsMoving = elbowIsMoving;
  prev_wristIsMoving = wristIsMoving;

   if (Serial.available()) {
     /*if (firstScan) {
      Serial.println("1,1,1,1");
      firstScan = false;
    }*/
    char chr = Serial.read();
    if (chr == '\n') {
      parseCommand(serialData);
      serialData = "";
    } else {
      serialData += chr;
    }
  }

  baseIsMoving = base.run();
  shoulderIsMoving = shoulder.run();
  elbowIsMoving = elbow.run();
  wristIsMoving = wrist.run();

  if ((prev_baseIsMoving || prev_elbowIsMoving || prev_shoulderIsMoving || prev_wristIsMoving)  
      && (!baseIsMoving && !shoulderIsMoving && !elbowIsMoving && !wristIsMoving)) {
      Serial.println("1,1,1,1"); //ASSUMING STEPS ARE NEVER LOST - WE ONLY SEND DATA WHEN ALL MOVEMENT IS DONE
  }
}

void parseCommand(String cmd) {
  cmd += ';';
  int lastIdx = 0;
  while (true) {
    int nextIdx = cmd.indexOf(';', lastIdx);
    if (nextIdx == -1) break;

    String token = cmd.substring(lastIdx, nextIdx);
    char axis = token.charAt(0);
    double position = token.substring(1).toDouble();

    if (axis == 'b') {
      moveAxisPrismatic(base, BASE_STEPS_PER_REV, position);
    } else if (axis == 's') {
      moveAxisRevolute(shoulder, position, SHOULDER_STEPS_PER_REV, SHOULDER_GEAR_RATIO);
    } else if (axis == 'e') {
      moveAxisRevolute(elbow, position, ELBOW_STEPS_PER_REV, ELBOW_GEAR_RATIO);
    } else if (axis == 'w') {
      moveAxisRevolute(wrist, position, WRIST_STEPS_PER_REV, WRIST_GEAR_RATIO);
    } //else if (axis == 'g') {
      //controlGripper(position);
    lastIdx = nextIdx + 1;
  }
}

void moveAxisRevolute(AccelStepper& motor, double angle, double steps_per_rev, double gear_ratio) {
  // Convert angle to steps
  double steps = (angle / 360.0) * steps_per_rev * gear_ratio;

  //if (motor.distanceToGo() != 0) {
    motor.moveTo(steps);
  //}
}
void moveAxisPrismatic(AccelStepper& motor, double steps_per_rev, double distance) {
  // Convert distance to steps
  double steps = ((-1) * distance / BASE_SCREW_PITCH) * steps_per_rev;

  //if (motor.distanceToGo() != 0) {
    motor.moveTo(steps);
  //}
}

void controlGripper(int state){
  switch (state) {
    case 0: // RELEASE
      valve.write(0);
      pump.write(0);
      break;
    case 1: // PICKUP
      pump.write(180);
      valve.write(180);
      break;
    case 2: // HOLD
      pump.write(0);
      valve.write(180);
      break;
    default:
      valve.write(0);
      pump.write(0);
      break;
  }
}