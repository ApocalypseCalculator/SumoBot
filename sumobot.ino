#include <Wire.h>
#include "Adafruit_VL6180X.h"
#include <MMA8453_n0m1.h>

const int pwmRight = 3; 
const int dirRight = 12;  
const int pwmLeft = 11;
const int dirLeft = 13; 

const int echoLeft = 5;     // left ultrasonic sensor echo
const int trigLeft = 4;     // left ultrasonic sensor trigger
const int echoRight = 6;    // right ultrasonic sensor echo
const int trigRight = 7;    // right ultrasonic sensor trigger

const int ldrIn = A0;       // ldr input

// an object controlling the lidar sensor
Adafruit_VL6180X lidar;

MMA8453_n0m1 accel;

const int LEFT_SPEED_FWD = 160; // speed of motors (0-255)
const int RIGHT_SPEED_FWD = 96;    

const int LEFT_SPEED_BWD = 160;
const int RIGHT_SPEED_BWD = 80;

const int LEFT_SPEED_TURN = 160;
const int RIGHT_SPEED_TURN = 160;

const int LEFT_SPEED_BUMP = 80;
const int RIGHT_SPEED_BUMP = 80;

const int ACCEL_N = 4;

int leftSpeed = 0;          // current speed of left motor (0-255)
int rightSpeed = 0;         // current speed of right motor (0-255)

// sets up all of the pins and sets the start time
void setup() {
  Serial.begin(9600);
  pinMode(pwmLeft, OUTPUT);
  pinMode(dirLeft, OUTPUT);
  pinMode(pwmRight, OUTPUT);
  pinMode(dirRight, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(ldrIn, INPUT);
  accel.setI2CAddr(0x1C);
  accel.dataMode(true, 2);
  lidar = Adafruit_VL6180X();
  lidar.begin();
}

// ultrasonic readings
void senseDists(float* left, float* right) {
  static float leftWindow[5] = {0.0};
  static int leftPointer = 0;
  static float leftSum = 0.0;
  static float rightWindow[5] = {0.0};
  static int rightPointer = 0;
  static float rightSum = 0.0;
  // left
  digitalWrite(trigLeft, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigLeft, LOW);
  float output1 = 0.017 * pulseIn(echoLeft, HIGH);
  leftSum -= leftWindow[leftPointer];
  leftWindow[leftPointer] = max(output1, 400.0);
  leftSum += leftWindow[leftPointer];
  leftPointer = (leftPointer + 1) % 5;
  *left = leftSum / 5.0;
  // right 
  digitalWrite(trigRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigRight, LOW);
  float output2 = 0.017 * pulseIn(echoRight, HIGH);
  rightSum -= rightWindow[rightPointer];
  rightWindow[rightPointer] = max(output2, 400.0);
  rightSum += rightWindow[rightPointer];
  rightPointer = (rightPointer + 1) % 5;
  *right = rightSum / 5.0;
}

void resetDists() {
  for(int i = 0; i < 5; i++) {
    float a, b;
    senseDists(&a, &b);
  }
}

// takes a reading from the ldr
float senseLDR() {
  int ldrValue = analogRead(ldrIn);
  return ldrValue * (5.0 / 1023.0);
}

float senseLidar() {
  static int window[5] = {0};
  static int pointer = 0;
  static int sum = 0;
  float lux = lidar.readLux(VL6180X_ALS_GAIN_5); // TODO: change this
  uint8_t range = lidar.readRange();
  sum -= window[pointer];
  window[pointer] = (int) range;
  sum += window[pointer];
  pointer = (pointer + 1) % 5;
  return sum / 5.0;
}

void resetLidar() {
  for(int i = 0; i < 5; i++) {
    senseLidar();
  }
}

float senseAccel() {
  static int window[5] = {0};
  static int pointer = 0;
  static int sum = 0;
  accel.update();
  int output = accel.x();
  sum -= window[pointer];
  window[pointer] = output;
  sum += window[pointer];
  pointer = (pointer + 1) % 5;
  return sum / 5.0;
}

void resetAccel() {
  for(int i = 0; i < 5; ++i) {
    senseAccel();
  }
}

void stop() {
  int leftStep = leftSpeed / ACCEL_N;
  int rightStep = rightSpeed / ACCEL_N;
  for(int i = 0; i < ACCEL_N; i++) {
    leftSpeed -= leftStep;
    rightSpeed -= rightStep;
    analogWrite(pwmLeft, leftSpeed);
    analogWrite(pwmRight, rightSpeed);
    delay(10);
  }
  analogWrite(pwmLeft, 0);
  analogWrite(pwmRight, 0);
  leftSpeed = 0;
  rightSpeed = 0;
  delay(10);
}

void changeSpeed(int newSpeedLeft, int newDirLeft, int newSpeedRight, int newDirRight) {
  stop();
  digitalWrite(dirLeft, newDirLeft);
  digitalWrite(dirRight, newDirRight);
  int leftStep = newSpeedLeft / ACCEL_N;
  int rightStep = newSpeedRight / ACCEL_N;
  // speed up motors
  for(int i = 0; i < ACCEL_N; i++) {
    leftSpeed += leftStep;
    rightSpeed += rightStep;
    analogWrite(pwmLeft, leftSpeed);
    analogWrite(pwmRight, rightSpeed);
    delay(10);
  }
  analogWrite(pwmLeft, newSpeedLeft);
  analogWrite(pwmRight, newSpeedRight);
  leftSpeed = newSpeedLeft;
  rightSpeed = newSpeedRight;
  delay(10);
}

// makes the sumobot move forward
void moveForward() {
  changeSpeed(LEFT_SPEED_FWD, HIGH, RIGHT_SPEED_FWD, LOW);
}

// makes the sumobot move backward
void moveBackward() {
  changeSpeed(LEFT_SPEED_BWD, LOW, RIGHT_SPEED_BWD, HIGH);
}

// makes the sumobot turn left
void turnLeft() {
  changeSpeed(LEFT_SPEED_TURN, LOW, RIGHT_SPEED_TURN, LOW);
}

// makes the sumobot turn right
void turnRight() {
  changeSpeed(LEFT_SPEED_TURN, HIGH, RIGHT_SPEED_TURN, HIGH);
}

// turn left when being accel'd (bumped) from the right
// side getting bumped is right
void turnLeftBump() {
  changeSpeed(LEFT_SPEED_TURN, LOW, RIGHT_SPEED_BUMP, LOW);
}

void turnRightBump() {
  changeSpeed(LEFT_SPEED_BUMP, HIGH, RIGHT_SPEED_TURN, HIGH);
}

// the main loop of the program
void loop() {
  static int timer = 0;
  static int phase = 0;
  // phase 1 -> waiting for 5s
  // phase 2 -> doing things for 60 s
  // phase 3 -> stopped
  if(phase == 0) {
    phase = 1;
    timer = millis();
  } else if(phase == 1 && millis() - timer > 5000) {
    resetLidar();
    resetDists();
    resetAccel();
    phase = 2;
    timer = millis();
  } else if(phase == 2 && millis() - timer > 60000) {
    stop();
    phase = 3;
    timer = millis();
  } else if(phase == 2)  {
    float distLeft, distRight;
    senseDists(&distLeft, &distRight);
    float ldrOut = senseLDR();
    float lidarOut = senseLidar();
    float accelX = senseAccel();
    if(ldrOut > 4.7875) {
      // sensed edge in front, go back
      moveBackward();
    } else if(lidarOut > 52.00) {
      // sensed edge behind, go forward
      moveForward();
    } else if(distLeft < 100.0 && distRight < 100.0) {
      // detected enemy straight ahead
      moveForward();
    } else if(distLeft < 100.0) {
      // detected enemy to the left
      turnLeft();
    } else if(distRight < 100.0) {
      // detected enemy to the right
      turnRight();
    } else if(lidarOut < 46.00) {
      // detected enemy behind
      moveBackward();
    } else if(accelX > 75.0) {
      // detected enemy pushing from right
      turnLeftBump();
    } else if(accelX < -75.0) {
      // detected enemy pushing from left
      turnRightBump();
    } else {
      // try to hunt for an enemy arbitrarily
      turnLeft();
    }
    // possibly add delay here, depending on test
  }
}
