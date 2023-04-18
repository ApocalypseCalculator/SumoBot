// Names: Leyang Zou, Patrick Lin, Ray Wang
// Teacher: Mr. Wong
// Date: 20/03/23
// Purpose: This program for Arduino controls the movement and sensing of the SumoBot.

// import libraries for the lidar sensor
#include <Wire.h>
#include "Adafruit_VL6180X.h"

// import library for the accelerometer
#include <MMA8453_n0m1.h>

const int pwmRight = 3;     // left motor speed control
const int dirRight = 12;    // left motor direction
const int pwmLeft = 11;     // right motor speed control
const int dirLeft = 13;     // right motor direction

const int echoLeft = 5;     // left ultrasonic sensor echo
const int trigLeft = 4;     // left ultrasonic sensor trigger
const int echoRight = 6;    // right ultrasonic sensor echo
const int trigRight = 7;    // right ultrasonic sensor trigger

const int ldrIn = A0;       // ldr input
const int ledIn = 8;        // the led for the ldr

// an object controlling the lidar sensor
Adafruit_VL6180X lidar;

// an object controlling the accelerometer
MMA8453_n0m1 accel;

const int LEFT_SPEED_FWD = 160;     // speed of left motor going forward
const int RIGHT_SPEED_FWD = 96;     // speed of right motor going forward

const int LEFT_SPEED_BWD = 160;     // speed of left motor going backward
const int RIGHT_SPEED_BWD = 80;     // speed of right motor going backward

const int LEFT_SPEED_TURN = 160;    // speed of left motor turning
const int RIGHT_SPEED_TURN = 160;   // speed of right motor turning

const int LEFT_SPEED_BUMP = 80;     // speed of left motor turning when being bumped
const int RIGHT_SPEED_BUMP = 80;    // speed of right motor turning when being bumpeed

const int ACCEL_N = 4;      // the number of steps to slow down the motor by

int leftSpeed = 0;          // current speed of left motor (0-255)
int rightSpeed = 0;         // current speed of right motor (0-255)

// sets up all of the pins and sensors for the SumoBot
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
  pinMode(ledIn, OUTPUT);
  // initialize the accelerometer
  accel.setI2CAddr(0x1C);
  accel.dataMode(true, 2);
  // initialize the lidar sensor
  lidar = Adafruit_VL6180X();
  lidar.begin();
}

// taking readings for the ultrasonic sensor
// left and right are pointers to the outputs of the left and right sensors
// the output is a rolling average of the last 5 measurements
void senseDists(float* left, float* right) {
  // an array of the last 5 measurements for the left sensor
  static float leftWindow[5] = {0.0};
  // the pointer to the current measurement for the left sensor
  static int leftPointer = 0;
  // the current sum of all measurements for the left sensor
  static float leftSum = 0.0;
  // an array of the last 5 measurements for the right sensor
  static float rightWindow[5] = {0.0};
  // the pointer to the current measurement for the right sensor
  static int rightPointer = 0;
  // the current sum of all measurements for the right sensor
  static float rightSum = 0.0;
  // take measurement from left sensor
  digitalWrite(trigLeft, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigLeft, LOW);
  // convert measurement to cm
  float output1 = 0.017 * pulseIn(echoLeft, HIGH);
  // add it to the rolling average
  leftSum -= leftWindow[leftPointer];
  // set the max measurement to 400 cm since the sensor frequently has errors
  leftWindow[leftPointer] = min(output1, 400.0);
  leftSum += leftWindow[leftPointer];
  leftPointer = (leftPointer + 1) % 5;
  // store the average in the output variable
  *left = leftSum / 5.0;
  // take measurement from right sensor
  digitalWrite(trigRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigRight, LOW);
  // convert measurement to cm
  float output2 = 0.017 * pulseIn(echoRight, HIGH);
  // add it to the rolling average
  rightSum -= rightWindow[rightPointer];
  rightWindow[rightPointer] = min(output2, 400.0);
  rightSum += rightWindow[rightPointer];
  rightPointer = (rightPointer + 1) % 5;
  // store the average in the output variable
  *right = rightSum / 5.0;
}

// resets the past measurements for the ultrasonic sensors
void resetDists() {
  // there are 5 total measurements in the array
  for(int i = 0; i < 5; i++) {
    float a, b; // temporary variables
    senseDists(&a, &b);
  }
}

// takes a reading from the ldr
float senseLDR() {
  // read the ldr pin
  int ldrValue = analogRead(ldrIn);
  return ldrValue * (5.0 / 1023.0);
}

// takes a reading from the lidar sensor
// the output is a rolling average of the last 5 measurements
float senseLidar() {
  // an array of the last 5 measurements
  static int window[5] = {0};
  // a pointer to the current measurement
  static int pointer = 0;
  // the sum of the last 5 measurements
  static int sum = 0;
  // read the distance in mm
  float lux = lidar.readLux(VL6180X_ALS_GAIN_2_5);
  uint8_t range = lidar.readRange();
  // add it to the rolling average
  sum -= window[pointer];
  window[pointer] = (int) range;
  sum += window[pointer];
  pointer = (pointer + 1) % 5;
  // return the average measurement
  return sum / 5.0;
}

// resets the past measurements for the lidar sensor
void resetLidar() {
  // there are 5 measurements
  for(int i = 0; i < 5; i++) {
    senseLidar();
  }
}

// takes an x component reading from the accelerometer
// the output is a rolling average of the last 5 measurements
float senseAccel() {
  // an array of the last 16 measurements
  static int window[16] = {0};
  // a pointer to the current measurements
  static int pointer = 0;
  // the sum of the last 16 measurements
  static int sum = 0;
  // update the value of the accelerometer
  accel.update();
  int output = accel.x();
  // bound the output to correct errors
  if(output > 0) output = min(output, 200);
  else output = max(output, -200);
  // add to the rolling average
  sum -= window[pointer];
  window[pointer] = output;
  sum += window[pointer];
  pointer = (pointer + 1) % 16;
  // return the average value
  return sum / 16.0;
}

// resets the accelerometer measurements
void resetAccel() {
  // there are 16 measurements
  for(int i = 0; i < 16; ++i) {
    senseAccel();
  }
}

// stops the motors gradually
void stop() {
  // the amount of speed to decrease the left motor by each step
  int leftStep = leftSpeed / ACCEL_N;
  // the amount of speed to decrease the right motor by each step
  int rightStep = rightSpeed / ACCEL_N;
  // iterate over the number of steps
  for(int i = 0; i < ACCEL_N; i++) {
    // decrease the current speeds by the step
    leftSpeed -= leftStep;
    rightSpeed -= rightStep;
    // set the speed of the motors
    analogWrite(pwmLeft, leftSpeed);
    analogWrite(pwmRight, rightSpeed);
    delay(10);
  }
  // set the final speeds to 0
  analogWrite(pwmLeft, 0);
  analogWrite(pwmRight, 0);
  leftSpeed = 0;
  rightSpeed = 0;
  delay(10);
}

// change the speeds and directions of the motors
// newSpeedLeft is the new speed of the left motor
// newDirLeft is the new direction of the left motor
// newSpeedRight is the new speed of the right motor
// newDirRight is the new direction of the right motor
void changeSpeed(int newSpeedLeft, int newDirLeft, int newSpeedRight, int newDirRight) {
  // first stop the motors
  stop();
  // change the directions
  digitalWrite(dirLeft, newDirLeft);
  digitalWrite(dirRight, newDirRight);
  // the amount to increase the left motor's speed each step
  int leftStep = newSpeedLeft / ACCEL_N;
  // the amount to increase the right motor's speed each step
  int rightStep = newSpeedRight / ACCEL_N;
  // speed up motors in N steps
  for(int i = 0; i < ACCEL_N; i++) {
    // increase the speeds
    leftSpeed += leftStep;
    rightSpeed += rightStep;
    // set the speeds
    analogWrite(pwmLeft, leftSpeed);
    analogWrite(pwmRight, rightSpeed);
    delay(10);
  }
  // set the final speed
  analogWrite(pwmLeft, newSpeedLeft);
  analogWrite(pwmRight, newSpeedRight);
  leftSpeed = newSpeedLeft;
  rightSpeed = newSpeedRight;
  delay(10);
}

// makes the sumobot move forward
void moveForward() {
  changeSpeed(LEFT_SPEED_FWD, HIGH, RIGHT_SPEED_FWD, HIGH);
}

// makes the sumobot move backward
void moveBackward() {
  changeSpeed(LEFT_SPEED_BWD, LOW, RIGHT_SPEED_BWD, LOW);
}

// makes the sumobot turn left
void turnLeft() {
  changeSpeed(LEFT_SPEED_TURN, LOW, RIGHT_SPEED_TURN, HIGH);
}

// makes the sumobot turn right
void turnRight() {
  changeSpeed(LEFT_SPEED_TURN, HIGH, RIGHT_SPEED_TURN, LOW);
}

// make the sumobot turn left when it's getting pushed from the right
void turnLeftBump() {
  changeSpeed(LEFT_SPEED_TURN, LOW, RIGHT_SPEED_BUMP, HIGH);
}

// make the sumobot turn right when it's getting pushed form the left
void turnRightBump() {
  changeSpeed(LEFT_SPEED_BUMP, HIGH, RIGHT_SPEED_TURN, LOW);
}

// the main loop of the program
void loop() {
  // the time in ms elapsed since the start of the phase
  static int timer = 0;
  // the current phase of the program
  // phase 1 -> waiting for 10 s
  // phase 2 -> run the program for 60 s
  // phase 3 -> the SumoBot is turned off
  static int phase = 0;
  if(phase == 0) {
    // change the phase to 1 at the start of the program
    phase = 1;
    timer = millis();
  } else if(phase == 1 && millis() - timer > 5000) {
    // when 10 seconds have elapsed, start the program
    // reset all the sensors
    resetLidar();
    resetDists();
    resetAccel();
    phase = 2;
    timer = millis();
  } else if(phase == 2 && millis() - timer > 60000) {
    // when 60 seconds have elapsed, stop the motors and stop the program
    stop();
    // turn off led
    digitalWrite(ledIn, LOW);
    phase = 3;
    timer = millis();
  } else if(phase == 2)  {
    // while the program is running
    // turn on the led
    digitalWrite(ledIn, HIGH);
    // sense the distances using ultrasonic sensors
    float distLeft, distRight;
    senseDists(&distLeft, &distRight);
    //Serial.print(distLeft);
    //Serial.print(" ");
    //Serial.println(distRight);
    // take readings from LDR, lidar, and accelerometer
    float ldrOut = senseLDR();
    float lidarOut = senseLidar();
    float accelX = senseAccel();
    Serial.println(lidarOut);
    if(ldrOut > 4.7875) {
      // sensed edge in front, go back
      Serial.println(0);
      moveBackward();
    } else if(lidarOut > 52.00) {
      // sensed edge behind, go forward
      Serial.println(1);
      moveForward();
    } else if(distLeft < 100.0 && distRight < 100.0) {
      // detected enemy straight ahead
      Serial.println(2);
      moveForward();
    } else if(distLeft < 100.0) {
      // detected enemy to the left
      Serial.println(3);
      turnLeft();
    } else if(distRight < 100.0) {
      // detected enemy to the right
      Serial.println(4);
      turnRight();
    } else if(lidarOut < 46.00) {
      // detected enemy behind
      Serial.println(5);
      moveBackward();
    } else if(accelX > 75.0) {
      // detected enemy pushing from right
      Serial.println(6);
      turnLeftBump();
    } else if(accelX < -75.0) {
      // detected enemy pushing from left
      Serial.println(7);
      turnRightBump();
    } else {
      // try to hunt for an enemy arbitrarily
      Serial.println(8);
      turnLeft();
    }
    delay(10);
  }
}
