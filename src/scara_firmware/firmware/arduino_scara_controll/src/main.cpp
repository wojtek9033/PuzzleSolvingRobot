#include <Arduino.h>
#include <AccelStepper.h>

#define MOTOR_STEPS 200

#define BASE_STEP_PIN 0
#define BASE_DIR_PIN 0
#define SHOULDER_STEP_PIN 0
#define SHOULDER_DIR_PIN 0
#define ELBOW_STEP_PIN 0
#define ELBOW_DIR_PIN 0
#define WRIST_PIN 0

AccelStepper base(AccelStepper::DRIVER, BASE_STEP_PIN, BASE_DIR_PIN);
AccelStepper shoulder(AccelStepper::DRIVER, SHOULDER_STEP_PIN, SHOULDER_DIR_PIN);
AccelStepper elbow(AccelStepper::DRIVER, ELBOW_STEP_PIN, ELBOW_DIR_PIN);

uint8_t idx = 0;
uint8_t value_idx = 0;
char value[4] = "000";

const double MICROSTEPS = 16.0; // Number of microsteps
const double MOTOR_STEPS_PER_REV = 200.0; // Number of steps per revolution for the motor
const double STEPS_PER_REV = MOTOR_STEPS * MICROSTEPS; // Total steps per revolution

const double BASE_SCREW_PITCH = 2.0; // Pitch of the screw in mm  
const double SHOULDER_GEAR_RATIO = 4.5; // Gear ratio for the shoulder motor
const double ELBOW_GEAR_RATIO = 7.28; // Gear ratio for the elbow motor

// Status of axis
bool baseIsMoving = false;
bool shoulderIsMoving = false;
bool elbowIsMoving = false;

// For slope detection
bool prev_baseIsMoving = false;
bool prev_shoulderIsMoving = false;
bool prev_elbowIsMoving = false;

void moveAxisRevolute(AccelStepper& motor, double angle);
void moveAxisPrismatic(AccelStepper& motor, double distance);

void setup() {

  base.setCurrentPosition(0); 
  shoulder.setCurrentPosition(0);
  elbow.setCurrentPosition(0);

  base.setMaxSpeed(STEPS_PER_REV * 2.0);
  shoulder.setMaxSpeed(STEPS_PER_REV  * 2.0);
  elbow.setMaxSpeed(STEPS_PER_REV * 2.0);

  base.setAcceleration(STEPS_PER_REV * 4.0);
  shoulder.setAcceleration(STEPS_PER_REV * 4.0);
  elbow.setAcceleration(STEPS_PER_REV * 4.0);
  

  Serial.begin(115200);
  Serial.setTimeout(1);
  Serial.println("SCARA Arduino Controller ready to receive commands!");
}

void loop() {
  prev_baseIsMoving = baseIsMoving;
  prev_shoulderIsMoving = shoulderIsMoving;
  prev_elbowIsMoving = elbowIsMoving;

  if(Serial.available()) {
    char chr = Serial.read();

    if (chr == 'b') {
      idx = 0;
      value_idx = 0;
    } else if (chr == 's') {
      idx = 1;
      value_idx = 0;
    } else if (chr == 'e') {
      idx = 2;
      value_idx = 0;
    } else if (chr == ',') {
      int angle = atoi(value);
      if (idx == 0) {
        moveAxisPrismatic(base, angle);
        baseIsMoving = base.run();
      } else if (idx == 1) {
        moveAxisRevolute(shoulder, angle);
        shoulderIsMoving = shoulder.run();
      } else if (idx == 2) {
        moveAxisRevolute(elbow, angle);
        elbowIsMoving = elbow.run();
      }
      value[0] = '0';
      value[1] = '0';
      value[2] = '0';
      value[3] = '\0';
    } else {
      value[value_idx] = chr;
      value_idx++;
    }

    if (prev_baseIsMoving && !baseIsMoving) {
      Serial.print("Base motor actuated: SUCCESS");
    }
    if (prev_shoulderIsMoving && !shoulderIsMoving) {
      Serial.print("Shoulder motor actuated: SUCCESS");
    }
    if (prev_elbowIsMoving && !elbowIsMoving) {
      Serial.print("Elbow motor actuated: SUCCESS");
    }

  }
}
void moveAxisRevolute(AccelStepper& motor, double angle) {
  // Convert angle to steps
  double steps = (angle / 360.0) * STEPS_PER_REV;

  while (motor.distanceToGo() != 0) {
    motor.moveTo(steps);
  }
}
void moveAxisPrismatic(AccelStepper& motor, double distance) {
  // Convert distance to steps
  double steps = (distance / BASE_SCREW_PITCH) * STEPS_PER_REV;

  while (motor.distanceToGo() != 0) {
    motor.moveTo(steps);
  }
}