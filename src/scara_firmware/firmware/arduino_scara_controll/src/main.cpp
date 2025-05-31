#include <Arduino.h>

#include <AccelStepper.h>

#define MOTOR_STEPS 200

#define BASE_STEP_PIN 2
#define BASE_DIR_PIN 3
#define SHOULDER_STEP_PIN 4
#define SHOULDER_DIR_PIN 5
#define ELBOW_STEP_PIN 6
#define ELBOW_DIR_PIN 7
#define WRIST_STEP_PIN 8
#define WRIST_DIR_PIN 9

// clockwise rotation means negative angle
AccelStepper base(AccelStepper::DRIVER, BASE_STEP_PIN, BASE_DIR_PIN);
AccelStepper shoulder(AccelStepper::DRIVER, SHOULDER_STEP_PIN, SHOULDER_DIR_PIN);
AccelStepper elbow(AccelStepper::DRIVER, ELBOW_STEP_PIN, ELBOW_DIR_PIN);
AccelStepper wrist(AccelStepper::DRIVER, WRIST_STEP_PIN, WRIST_DIR_PIN);

const double MICROSTEPS = 16.0; // Number of microsteps
const double MOTOR_STEPS_PER_REV = 200.0; // Number of steps per revolution for the motor
const double STEPS_PER_REV = MOTOR_STEPS * MICROSTEPS; // Total steps per revolution

const double BASE_SCREW_PITCH = 2.0; // Pitch of the screw in mm  
const double SHOULDER_GEAR_RATIO = 4.5; // Gear ratio for the shoulder motor
const double ELBOW_GEAR_RATIO = 7.28; // Gear ratio for the elbow motor
const double WRIST_GEAR_RATIO = 3.0;

// Status of axis
bool baseIsMoving = false;
bool shoulderIsMoving = false;
bool elbowIsMoving = false;
bool wristwIsMoving = false;

// For slope detection
bool prev_baseIsMoving = false;
bool prev_shoulderIsMoving = false;
bool prev_elbowIsMoving = false;
bool prev_wristIsMoving = false;

// Buffer for receiving a command
String serialData = "";
String baseStatus = "";
String shoulderStatus = "";
String elbowStatus = "";
String wristStatus = "";
String status = "";

void moveAxisRevolute(AccelStepper& motor, double angle, double gear_ratio);
void moveAxisPrismatic(AccelStepper& motor, double distance);
void parseCommand(String cmd);

void setup() {

  base.setCurrentPosition(0); 
  shoulder.setCurrentPosition(0);
  elbow.setCurrentPosition(0);
  wrist.setCurrentPosition(0);

  base.setMaxSpeed(STEPS_PER_REV * 6.0);
  shoulder.setMaxSpeed(STEPS_PER_REV  * 2.0);
  elbow.setMaxSpeed(STEPS_PER_REV * 2.0);
  wrist.setMaxSpeed(STEPS_PER_REV);

  base.setAcceleration(STEPS_PER_REV * 4.0);
  shoulder.setAcceleration(STEPS_PER_REV * 4.0);
  elbow.setAcceleration(STEPS_PER_REV * 4.0);
  wrist.setAcceleration(STEPS_PER_REV);

  Serial.begin(9600);
}

void loop() {
  prev_baseIsMoving = baseIsMoving;
  prev_shoulderIsMoving = shoulderIsMoving;
  prev_elbowIsMoving = elbowIsMoving;
  prev_wristIsMoving = wristwIsMoving;

  if (Serial.available()) {
    char chr = Serial.read();
    Serial.print(chr);
    if (chr == '\n') {
      parseCommand(serialData);
      Serial.print("Data is: ");
      Serial.print(serialData);
      Serial.print('\n');
      serialData = "";
    } else {
      serialData += chr;
    }
  }

  baseIsMoving = base.run();
  shoulderIsMoving = shoulder.run();
  elbowIsMoving = elbow.run();

  if ((prev_baseIsMoving || prev_elbowIsMoving || prev_shoulderIsMoving || prev_wristIsMoving) 
      && (!baseIsMoving && !shoulderIsMoving && !elbowIsMoving && !wristwIsMoving)) {
      Serial.println("1,1,1,1"); //ASSUMING STEPS ARE NEVER LOST - WE ONLY SEND DATA WHEN ALL MOVEMENT IS DONE
  }
}

void parseCommand(String cmd) {
  cmd += ',';
  int lastIdx = 0;
  while (true) {
    int nextIdx = cmd.indexOf(',', lastIdx);
    if (nextIdx == -1) break;

    String token = cmd.substring(lastIdx, nextIdx);
    char axis = token.charAt(0);
    int position = token.substring(1).toInt();

    if (axis == 'b') {
      Serial.print("Axis base, position: ");
      Serial.print(position);
      Serial.print("\n");
      moveAxisPrismatic(base, position);
    } else if (axis == 's') {
      Serial.print("Axis shoulder, position: ");
      Serial.print(position);
      Serial.print("\n");
      moveAxisRevolute(shoulder, position, SHOULDER_GEAR_RATIO);
    } else if (axis == 'e') {
      Serial.print("Axis elbow, position: ");
      Serial.print(position);
      Serial.print("\n");
      moveAxisRevolute(elbow, position, ELBOW_GEAR_RATIO);
    } else if (axis == 'w') {
      Serial.print("Axis wrist, position: ");
      Serial.print(position);
      Serial.print("\n");
      moveAxisRevolute(wrist, position, WRIST_GEAR_RATIO);
    }
    lastIdx = nextIdx + 1;
  }
}

void moveAxisRevolute(AccelStepper& motor, double angle, double gear_ratio) {
  // Convert angle to steps
  double steps = (angle / 360.0) * STEPS_PER_REV * gear_ratio;

  //if (motor.distanceToGo() != 0) {
    motor.moveTo(steps);
  //}
}
void moveAxisPrismatic(AccelStepper& motor, double distance) {
  // Convert distance to steps
  double steps = ((-1) * distance / BASE_SCREW_PITCH) * STEPS_PER_REV;

  //if (motor.distanceToGo() != 0) {
    motor.moveTo(steps);
  //}
}