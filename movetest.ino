// Names: Leyang Zou, Patrick Lin, Ray Wang
// Teacher: Mr. Wong
// Date: 10/03/23
// Purpose: This program for Arduino controls the movement and sensing of the SumoBot.

// import libraries for the lidar sensor
#include <Wire.h>
#include "Adafruit_VL6180X.h"

const int pwmLeft = 3;      // left motor speed control
const int dirLeft = 12;     // left motor direction
const int pwmRight = 11;    // right motor speed control
const int dirRight = 13;    // right motor direction

const int echoLeft = 2;     // left ultrasonic sensor echo
const int trigLeft = 3;     // left ultrasonic sensor trigger
const int echoRight = 4;    // right ultrasonic sensor echo
const int trigRight = 5;    // right ultrasonic sensor trigger

const int ldrIn = A0;       // ldr input

int startTime;              // tracks the start time of the program

const int SPEED = 160;      // speed of motors (0-255)
const int SLOW_STEPS = 4;   // number of steps to slow down

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

void changeDir(int newLeftDir, int newRightDir) {
  // slow down motors
  int stepSize = SPEED / SLOW_STEPS;
  for(int i = 1; i <= SLOW_STEPS; i++) {
    analogWrite(pwmLeft, SPEED - i * stepSize);
    analogWrite(pwmRight, SPEED - i * stepSize);
    delay(10);
  }
  // change direction
  digitalWrite(dirLeft, newLeftDir);
  digitalWrite(dirRight, newRightDir);
  // speed up motors
  for(int i = SLOW_STEPS - 1; i >= 0; i--) {
    analogWrite(pwmLeft, SPEED - i * stepSize);
    analogWrite(pwmRight, SPEED - i * stepSize);
    delay(10);
  }
}

/*
int leftFwd = LOW;          // direction of left motor (HIGH is forward)
int rightFwd = LOW;         // direction of right motor (LOW is forward)
*/

// makes the sumobot move forward
void moveForward() {
  changeDir(HIGH, LOW); // left forward, right forward
}

// makes the sumobot move backward
void moveBackward() {
  changeDir(LOW, HIGH); // left backward, right backward
}

// makes the sumobot turn left
void turnLeft() {
  changeDir(LOW, LOW); // left backward, right forward
}

// makes the sumobot turn right
void turnRight() {
  changeDir(HIGH, HIGH); // left forward, right backward
}

// the main loop of the program
void loop() {
  // 5 second initial delay
  if(millis() - startTime > 5000) {
    moveForward();
    delay(500);
    moveBackward();
    delay(500);
  }
}
