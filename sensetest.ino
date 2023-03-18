// Names: Leyang Zou, Patrick Lin, Ray Wang
// Teacher: Mr. Wong
// Date: 10/03/23
// Purpose: This program for Arduino controls the movement and sensing of the SumoBot.

// import libraries for the lidar sensor
#include <Wire.h>
#include "Adafruit_VL6180X.h"
#include <MMA8453_n0m1.h>

const int echoLeft = 5;     // left ultrasonic sensor echo
const int trigLeft = 4;     // left ultrasonic sensor trigger
const int echoRight = 6;    // right ultrasonic sensor echo
const int trigRight = 7;    // right ultrasonic sensor trigger

const int ldrIn = A0;       // ldr input

// an object controlling the lidar sensor
Adafruit_VL6180X lidar;

MMA8453_n0m1 accel;

int startTime;              // tracks the start time of the program

// sets up all of the pins and sets the start time
void setup() {
  Serial.begin(9600);
  pinMode(echoLeft, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(ldrIn, INPUT);
  startTime = millis();
  accel.setI2CAddr(0x1C);
  accel.dataMode(true, 2); //2, 4, 8g range
}

// ultrasonic readings
void senseDist(float* left, float* right) {
  digitalWrite(trigLeft, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigLeft, LOW);
  float output1 = pulseIn(echoLeft, HIGH);
  *left = 0.017 * output1;
  digitalWrite(trigRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigRight, LOW);
  float output2 = pulseIn(echoRight, HIGH);
  *right = 0.017 * output2;
}

// takes a reading from the ldr
float senseLDR() {
  int ldrValue = analogRead(ldrIn);
  return ldrValue * (5.0 / 1023.0);
}

float senseLidar() {
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
  return (float) range;
}

// the main loop of the program
void loop() {
  // 5 second initial delay
  if(millis() - startTime > 5000) {
    float meas = senseLDR();
    Serial.println(meas);
    delay(500);
  }
}
