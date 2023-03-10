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

// maxmimum distance for an object to count as detected
const float distThreshold = 30.0; 

// an object controlling the lidar sensor
Adafruit_VL6180X lidar = Adafruit_VL6180X();

int leftSpeed = 0;          // speed of left motor (0-255)
int rightSpeed = 0;         // speed of right motor (0-255)
int leftFwd = LOW;          // direction of left motor (HIGH is forward)
int rightFwd = LOW;         // direction of right motor (HIGH is forward)

int startTime;              // tracks the start time of the program

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

// takes a reading from the left ultrasonic sensor
float senseLeft() {
  float sum = 0.0;
  // take 5 measurements for noise reduction
  for(int i = 0; i < 5; ++i) {
    digitalWrite(trigLeft, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigLeft, LOW);
    float output = pulseIn(echoLeft, HIGH);
    sum += output;
  }
  // take the average of the measurements and convert to cm
  return 0.017 * (sum / 5);
}

// takes a reading from the right ultrasonic sensor
float senseRight() {
  float sum = 0.0;
  // take 5 measurements for noise reduction
  for(int i = 0; i < 5; ++i) {
    digitalWrite(trigRight, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigRight, LOW);
    float output = pulseIn(echoRight, HIGH);
    sum += output;
  }
  // take the average of the measurements and convert to cm
  return 0.017 * (sum / 5);
}

// takes a reading from the ldr
float senseLDR() {
  int sum = 0;
  // take 5 measurements for noise reduction
  for(int i = 0; i < 5; ++i) {
    int ldrValue = analogRead(ldrIn);
    sum += ldrValue;
    delay(10);
  }
  // take the average of the measurements
  return (sum / 5.0) * (5.0 / 1023.0);
}

// takes a reading from the lidar sensor
// todo add more comments here
float senseLidar() {
  int sum = 0;
  // take 5 measurements for noise reduction
  for(int i = 0; i < 5; ++i) {
    float lux = lidar.readLux(VL6180X_ALS_GAIN_5);
    uint8_t range = lidar.readRange();
    uint8_t status = lidar.readRangeStatus();
    // keep trying until the sensor works
    while(status != VL6180X_ERROR_NONE) {
      delay(10);
      lux = lidar.readLux(VL6180X_ALS_GAIN_5);
      range = lidar.readRange();
      status = lidar.readRangeStatus();
    }
    sum += range;
    delay(10);
  }
  // take the average of the measurements
  return sum / 5.0;
}

// changes the speed of the motors gradually to not overwhelm the motors
// newSpeedLeft and newFwdLeft are the new speed and direction for the left motor
// newSpeedRight and newFwdRight are the new speed and direction for the right motor
void changeSpeed(int newSpeedLeft, int newFwdLeft, int newSpeedRight, int newFwdRight) {
  if(newFwdLeft != leftFwd || newFwdRight != rightFwd) {
    // change directions by reducing speed to 0
    while(leftSpeed > 0 || rightSpeed > 0) {
      if(leftSpeed > 0) {
        // decrease left speed
        --leftSpeed;
        digitalWrite(dirLeft, leftFwd);
        analogWrite(pwmLeft, leftSpeed);
      }
      if(rightSpeed > 0) {
        // decrease right speed
        --rightSpeed;
        digitalWrite(dirRight, leftFwd);
        analogWrite(pwmRight, leftSpeed);
      }
      delay(1);
    }
    // update the directions
    leftFwd = newFwdLeft;
    rightFwd = newFwdRight;
  }
  // now update the speeds to the required value
  while(leftSpeed < newSpeedLeft || rightSpeed < newSpeedRight) {
    if(leftSpeed < newSpeedLeft) {
      // increase left speed
      ++leftSpeed;
      digitalWrite(dirLeft, leftFwd);
      analogWrite(pwmLeft, leftSpeed);
    }
    if(rightSpeed < newSpeedRight) {
      // increase right speed
      ++rightSpeed;
      digitalWrite(dirRight, rightFwd);
      analogWrite(pwmRight, rightSpeed);
    }
    delay(1);
  }
}

// makes the sumobot move forward
void moveForward() {
  changeSpeed(230, HIGH, 230, HIGH);
}

// makes the sumobot move backward
void moveBackward() {
  changeSpeed(230, LOW, 230, LOW);
}

// makes the sumobot turn left
void turnLeft() {
  changeSpeed(250, LOW, 250, HIGH);
}

// makes the sumobot turn right
void turnRight() {
  changeSpeed(250, HIGH, 250, LOW);
}

// the main loop of the program
void loop() {
  // 5 second initial delay
  if(millis() - startTime > 5000) {
    // get ultrasonic sensor measurements
    float distLeft = senseLeft();
    float distRight = senseRight();
    Serial.print("Distances detected: ");
    Serial.print(distLeft);
    Serial.print(" ");
    Serial.print(distRight);
    Serial.println();
    // until the enemy is sensed, continue turning
    while(distLeft > distThreshold || distRight > distThreshold) {
      // turn left or right dependening on enemy direction
      Serial.print("Distances detected: ");
      Serial.print(distLeft);
      Serial.print(" ");
      Serial.print(distRight);
      Serial.println();
      if(distLeft <= distThreshold) {
        // if enemy is on the left, turn left
        Serial.println("Turning left");
        turnLeft();
        delay(100);
      } else {
        // if enemy is on the right or completely undetected, turn right
        Serial.println("Turning right");
        turnRight();
        delay(100);
      }
      // check ldr reading
      float ldrOut = senseLDR();
      Serial.print("LDR value: ");
      Serial.println(ldrOut);
      while(abs(ldrOut - 4.78) > abs(ldrOut - 4.47)) {
        // likely white surface in front, so move backward
        Serial.println("Moving backward");
        moveBackward();
        delay(100);
        ldrOut = senseLDR();
        Serial.print("LDR value: ");
        Serial.println(ldrOut);
      }
      // check light sensor reading
      float lidarOut = senseLidar();
      Serial.print("Lidar value: ");
      Serial.println(lidarOut);
      // todo: just use either < 29 OR > 39
      while(lidarOut < 29.0 || lidarOut > 39.0) {
        // likely white surface behind, so move forward
        Serial.println("Moving forward");
        moveForward();
        delay(100);
        lidarOut = senseLidar();
        Serial.print("Lidar value: ");
        Serial.println(lidarOut);
      }
      distLeft = senseLeft();
      distRight = senseRight();
    }
    // after enemy has been sensed, move forward for a while
    Serial.println("Moving forward normally");
    moveForward();
    delay(200);
  }
}
