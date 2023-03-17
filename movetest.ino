// Names: Leyang Zou, Patrick Lin, Ray Wang
// Teacher: Mr. Wong
// Date: 10/03/23
// Purpose: This program for Arduino controls the movement and sensing of the SumoBot.

// import libraries for the lidar sensor
#include <Wire.h>
#include "Adafruit_VL6180X.h"

const int pwmLeft = 3;    // left motor speed control
const int dirLeft = 12;   // left motor direction
const int pwmRight = 11;  // right motor speed control
const int dirRight = 13;  // right motor direction

const int echoLeft = 2;   // left ultrasonic sensor echo
const int trigLeft = 3;   // left ultrasonic sensor trigger
const int echoRight = 4;  // right ultrasonic sensor echo
const int trigRight = 5;  // right ultrasonic sensor trigger

const int ldrIn = A0;  // ldr input

int startTime;  // tracks the start time of the program

const int SPEED = 160;       // speed of motors (0-255)
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
  int stepSize = SPEED / SLOW_STEPS;
  for (int i = 1; i <= SLOW_STEPS; i++) {
    analogWrite(pwmRight, SPEED - i * stepSize);
    analogWrite(pwmLeft, SPEED - i * stepSize);
    delay(10);
  }
  analogWrite(pwmRight, 0);
  analogWrite(pwmLeft, 0);
}

void changeDir(int newLeftDir, int newRightDir) {
  // slow down motors
  int stepSize = SPEED / SLOW_STEPS;
  for (int i = 1; i <= SLOW_STEPS; i++) {
    analogWrite(pwmRight, SPEED - i * stepSize);
    
    if(2 * i <= SLOW_STEPS) {
      analogWrite(pwmLeft, SPEED - i * 2 * stepSize);
    }
    delay(10);
  }
  // change direction
  analogWrite(pwmRight, 0);
  analogWrite(pwmLeft, 0);
  digitalWrite(dirRight, newRightDir);
  digitalWrite(dirLeft, newLeftDir);
  delay(10);
  // speed up motors
  for(int i = 1; i <= SLOW_STEPS; ++i) {
    analogWrite(pwmRight, i * stepSize);

    if(2 * i <= SLOW_STEPS) {
      analogWrite(pwmRight, 2 * i * stepSize);
    }
    delay(10);
  }
  analogWrite(pwmRight, SPEED);
  analogWrite(pwmLeft, SPEED);
}

// makes the sumobot move forward
void moveForward() {
  changeDir(LOW, HIGH);  // left forward, right forward
}

// makes the sumobot move backward
void moveBackward() {
  changeDir(HIGH, LOW);  // left backward, right backward
}

// makes the sumobot turn left
void turnLeft() {
  changeDir(HIGH, HIGH);  // left backward, right forward
}

// makes the sumobot turn right
void turnRight() {
  changeDir(LOW, LOW);  // left forward, right backward
}

int tmp = 0;

// the main loop of the program
void loop() {
  // 5 second initial delay
  int past = millis() - startTime;
  if (5000 < past && past < 10000) {
    if (tmp == 0) {
      digitalWrite(dirLeft, LOW);
      analogWrite(pwmLeft, SPEED);
      digitalWrite(dirRight, HIGH);
      analogWrite(pwmRight, SPEED);
      tmp = 1;
    } else {
      moveForward();
      delay(1000);
      moveBackward();
      delay(1000);
    }
  } else if (past > 10000) {
    if (tmp == 1) {
      stop();
      tmp = 2;
    }
  }
}
