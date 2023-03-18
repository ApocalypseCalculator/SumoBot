// Names: Leyang Zou, Patrick Lin, Ray Wang
// Teacher: Mr. Wong
// Date: 10/03/23
// Purpose: This program for Arduino controls the movement and sensing of the SumoBot.

// import libraries for the lidar sensor
#include <Wire.h>
#include "Adafruit_VL6180X.h"

const int pwmRight = 3; 
const int dirRight = 12;  
const int pwmLeft = 11;
const int dirLeft = 13; 

const int echoLeft = 2;   // left ultrasonic sensor echo
const int trigLeft = 3;   // left ultrasonic sensor trigger
const int echoRight = 4;  // right ultrasonic sensor echo
const int trigRight = 5;  // right ultrasonic sensor trigger

const int ldrIn = A0;  // ldr input

int startTime;  // tracks the start time of the program

const int LEFT_SPEED_FWD = 160; // speed of motors (0-255)
const int RIGHT_SPEED_FWD = 96;    

const int LEFT_SPEED_BWD = 160;
const int RIGHT_SPEED_BWD = 80;

const int SLOW_STEPS = 4;  // number of steps to slow down

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
  startTime = millis();
}

void stop() {
  int leftStep = LEFT_SPEED_BWD / SLOW_STEPS;
  int rightStep = RIGHT_SPEED_BWD / SLOW_STEPS;
  for (int i = 1; i <= SLOW_STEPS; i++) {
    analogWrite(pwmLeft, LEFT_SPEED_BWD - i * leftStep);
    analogWrite(pwmRight, RIGHT_SPEED_BWD - i * rightStep);
    delay(10);
  }
  analogWrite(pwmLeft, 0);
  analogWrite(pwmRight, 0);
}

void changeDirF2B(int newLeftDir, int newRightDir) {
  // slow down motors
  int leftStepFwd = LEFT_SPEED_FWD / SLOW_STEPS;
  int rightStepFwd = RIGHT_SPEED_FWD / SLOW_STEPS;
  for (int i = 1; i <= SLOW_STEPS; i++) {
    analogWrite(pwmLeft, LEFT_SPEED_FWD - i * leftStepFwd);
    analogWrite(pwmRight, RIGHT_SPEED_FWD - i * rightStepFwd);
    delay(10);
  }
  // change direction
  analogWrite(pwmLeft, 0);
  analogWrite(pwmRight, 0);
  digitalWrite(dirLeft, newLeftDir);
  digitalWrite(dirRight, newRightDir);
  delay(1000);
  int leftStepBwd = LEFT_SPEED_BWD / SLOW_STEPS;
  int rightStepBwd = RIGHT_SPEED_BWD / SLOW_STEPS;
  // speed up motors
  for(int i = 1; i <= SLOW_STEPS; ++i) {
    analogWrite(pwmLeft, i * leftStepBwd);
    analogWrite(pwmRight, i * rightStepBwd);
    delay(10);
  }
  analogWrite(pwmLeft, LEFT_SPEED_BWD);
  analogWrite(pwmRight, RIGHT_SPEED_BWD);
}

void changeDirB2F(int newLeftDir, int newRightDir) {
  // slow down motors
  int leftStepBwd = LEFT_SPEED_BWD / SLOW_STEPS;
  int rightStepBwd = RIGHT_SPEED_BWD / SLOW_STEPS;
  for (int i = 1; i <= SLOW_STEPS; i++) {
    analogWrite(pwmLeft, LEFT_SPEED_BWD - i * leftStepBwd);
    analogWrite(pwmRight, RIGHT_SPEED_BWD - i * rightStepBwd);
    delay(10);
  }
  // change direction
  analogWrite(pwmLeft, 0);
  analogWrite(pwmRight, 0);
  digitalWrite(dirLeft, newLeftDir);
  digitalWrite(dirRight, newRightDir);
  delay(1000);
  int leftStepFwd = LEFT_SPEED_FWD / SLOW_STEPS;
  int rightStepFwd = RIGHT_SPEED_FWD / SLOW_STEPS;
  // speed up motors
  for(int i = 1; i <= SLOW_STEPS; ++i) {
    analogWrite(pwmLeft, i * leftStepFwd);
    analogWrite(pwmRight, i * rightStepFwd);
    delay(10);
  }
  analogWrite(pwmLeft, LEFT_SPEED_FWD);
  analogWrite(pwmRight, RIGHT_SPEED_FWD);
}

/*
void changeDir(int newLeftDir, int newRightDir) {
  // slow down motors
  int leftStep = LEFT_SPEED / SLOW_STEPS;
  int rightStep = RIGHT_SPEED / SLOW_STEPS;
  for (int i = 1; i <= SLOW_STEPS; i++) {
    analogWrite(pwmLeft, LEFT_SPEED - i * leftStep);
    analogWrite(pwmRight, RIGHT_SPEED - i * rightStep);
    delay(10);
  }
  // change direction
  analogWrite(pwmLeft, 0);
  analogWrite(pwmRight, 0);
  digitalWrite(dirLeft, newLeftDir);
  digitalWrite(dirRight, newRightDir);
  delay(1000);
  // speed up motors
  for(int i = 1; i <= SLOW_STEPS; ++i) {
    analogWrite(pwmLeft, i * leftStep);
    analogWrite(pwmRight, i * rightStep);
    delay(10);
  }
  analogWrite(pwmLeft, LEFT_SPEED);
  analogWrite(pwmRight, RIGHT_SPEED);
}
*/

// makes the sumobot move forward
void moveForward() {
  changeDirB2F(HIGH, LOW);
  //changeDir(HIGH, LOW);  // left forward, right forward
}

// makes the sumobot move backward
void moveBackward() {
  changeDirF2B(LOW, HIGH);
  //changeDir(LOW, HIGH);  // left backward, right backward
}

// makes the sumobot turn left
void turnLeft() {
  //changeDir(LOW, LOW);  // left backward, right forward
}

// makes the sumobot turn right
void turnRight() {
  //changeDir(HIGH, HIGH);  // left forward, right backward
}

int tmp = 0;

// the main loop of the program
void loop() {
  // 5 second initial delay
  int past = millis() - startTime;
  if (5000 < past && past < 15000) {
    if (tmp == 0) {
      digitalWrite(dirLeft, HIGH);
      analogWrite(pwmLeft, LEFT_SPEED_FWD);
      digitalWrite(dirRight, LOW);
      analogWrite(pwmRight, RIGHT_SPEED_FWD);
      tmp = 1;
    } else {
      moveBackward();
      delay(1000);
      moveForward();
      delay(1000);
    }
  } else if (past > 15000) {
    if (tmp == 1) {
      stop();
      tmp = 2;
    }
  }
}
