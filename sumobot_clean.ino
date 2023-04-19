// Names: Leyang Zou, Patrick Lin, Ray Wang
// Teacher: Mr. Wong
// Date: 20/03/23
// Purpose: This program for Arduino controls the movement and sensing of the SumoBot.

// import libraries for the lidar sensor
#include <Wire.h>
#include "Adafruit_VL6180X.h"

const int pwmRight = 3;  // left motor speed control
const int dirRight = 12; // left motor direction
const int pwmLeft = 11;  // right motor speed control
const int dirLeft = 13;  // right motor direction

const int echoLeft = 5;  // left ultrasonic sensor echo
const int trigLeft = 4;  // left ultrasonic sensor trigger
const int echoRight = 6; // right ultrasonic sensor echo
const int trigRight = 7; // right ultrasonic sensor trigger

const int ldrIn = A0; // ldr input
const int ledIn = 8;  // the led for the ldr

const int led2Out = 9;

// an object controlling the lidar sensor
Adafruit_VL6180X lidar;

const int LEFT_SPEED_FWD = 100; // 160;     // speed of left motor going forward
const int RIGHT_SPEED_FWD = 100; // 96;     // speed of right motor going forward

const int LEFT_SPEED_BWD = 150; // 160;     // speed of left motor going backward
const int RIGHT_SPEED_BWD = 150; // 80;     // speed of right motor going backward

const int LEFT_SPEED_TURN = 120;  // 160;    // speed of left motor turning
const int RIGHT_SPEED_TURN = 120; // 160;   // speed of right motor turning

// left turn and move ()

const int ACCEL_N = 4; // the number of steps to slow down the motor by

int leftSpeed = 0;  // current speed of left motor (0-255)
int rightSpeed = 0; // current speed of right motor (0-255)

// sets up all of the pins and sensors for the SumoBot
void setup()
{
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
    pinMode(led2Out, OUTPUT);
    // initialize the lidar sensor
    lidar = Adafruit_VL6180X();
    lidar.begin();
}

// taking readings for the ultrasonic sensor
// left and right are pointers to the outputs of the left and right sensors
// the output is a rolling average of the last 5 measurements
void senseDists(float *left, float *right)
{
    // take measurement from left sensor
    digitalWrite(trigLeft, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigLeft, LOW);
    *left = 0.017 * pulseIn(echoLeft, HIGH);

    // take measurement from right sensor
    digitalWrite(trigRight, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigRight, LOW);
    *right = 0.017 * pulseIn(echoRight, HIGH);
}

// takes a reading from the ldr
float senseLDR()
{
    // read the ldr pin
    int ldrValue = analogRead(ldrIn);
    return ldrValue * (5.0 / 1023.0);
}

// takes a reading from the lidar sensor
// the output is a rolling average of the last 5 measurements
float senseLidar()
{
    uint8_t range = lidar.readRange();
    return range;
}

// stops the motors gradually
void stop()
{
    // the amount of speed to decrease the left motor by each step
    int leftStep = leftSpeed / ACCEL_N;
    // the amount of speed to decrease the right motor by each step
    int rightStep = rightSpeed / ACCEL_N;
    // iterate over the number of steps
    /*for (int i = 0; i < ACCEL_N; i++)
    {
        // decrease the current speeds by the step
        leftSpeed -= leftStep;
        rightSpeed -= rightStep;
        // set the speed of the motors
        analogWrite(pwmLeft, leftSpeed);
        analogWrite(pwmRight, rightSpeed);
    }*/
    // set the final speeds to 0
    analogWrite(pwmLeft, 0);
    analogWrite(pwmRight, 0);
    leftSpeed = 0;
    rightSpeed = 0;
}

// change the speeds and directions of the motors
// newSpeedLeft is the new speed of the left motor
// newDirLeft is the new direction of the left motor
// newSpeedRight is the new speed of the right motor
// newDirRight is the new direction of the right motor
void changeSpeed(int newSpeedLeft, int newDirLeft, int newSpeedRight, int newDirRight)
{
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
    /*for (int i = 0; i < ACCEL_N; i++)
    {
        // increase the speeds
        leftSpeed += leftStep;
        rightSpeed += rightStep;
        // set the speeds
        analogWrite(pwmLeft, leftSpeed);
        analogWrite(pwmRight, rightSpeed);
    }*/
    // set the final speed
    analogWrite(pwmLeft, newSpeedLeft);
    analogWrite(pwmRight, newSpeedRight);
    leftSpeed = newSpeedLeft;
    rightSpeed = newSpeedRight;
}

// makes the sumobot move forward
void moveForward()
{
    changeSpeed(LEFT_SPEED_FWD, HIGH, RIGHT_SPEED_FWD, HIGH);
}

// makes the sumobot move backward
void moveBackward()
{
    changeSpeed(LEFT_SPEED_BWD, LOW, RIGHT_SPEED_BWD, LOW);
}

// makes the sumobot turn left
void turnLeft()
{
    changeSpeed(LEFT_SPEED_TURN, LOW, RIGHT_SPEED_TURN, HIGH);
}

// makes the sumobot turn right
void turnRight()
{
    changeSpeed(LEFT_SPEED_TURN, HIGH, RIGHT_SPEED_TURN, LOW);
}

void turnMoveLeft()
{
    changeSpeed(LEFT_SPEED_TURN / 2, HIGH, RIGHT_SPEED_FWD, HIGH);
}

void turnMoveRight()
{
    changeSpeed(LEFT_SPEED_FWD, HIGH, RIGHT_SPEED_TURN / 2, HIGH);
}

// the main loop of the program
void loop()
{
    // the time in ms elapsed since the start of the phase
    static int timer = 0;
    // the current phase of the program
    // phase 1 -> waiting for 10 s
    // phase 2 -> run the program for 3 * 60 s
    // phase 3 -> the SumoBot is turned off
    static int phase = 0;
    static bool ldrledOn = false;
    if (phase == 0)
    {
        // change the phase to 1 at the start of the program
        phase = 1;
        timer = millis();
    }
    else if (phase == 1 && millis() - timer > 5000)
    {
        // when 10 seconds have elapsed, start the program
        phase = 2;
        timer = millis();
    }
    else if (phase == 2 && millis() - timer > 3 * 60000)
    {
        // when 3 * 60 seconds have elapsed, stop the motors and stop the program
        stop();
        if (ldrledOn)
        {
            // turn off led
            digitalWrite(ledIn, LOW);
            ldrledOn = false;
        }
        phase = 3;
        timer = millis();
    }
    else if (phase == 2)
    {
        // while the program is running
        // turn on the led
        if (!ldrledOn)
        {
            digitalWrite(ledIn, HIGH);
            ldrledOn = true;
        }
        // sense the distances using ultrasonic sensors
        float distLeft, distRight;
        senseDists(&distLeft, &distRight);
        // take readings from LDR, lidar, and accelerometer
        float ldrOut = senseLDR();
        float lidarOut = senseLidar();
        if (ldrOut > 4.7875)
        {
            // sensed edge in front, go back
            moveBackward();
        }
        else if (lidarOut > 55.00)
        {
            // sensed edge behind, go forward
            moveForward();
        }
        else if (lidarOut < 30.00)
        {
            // detected enemy behind
            moveBackward();
        }
        else if (distLeft < 75.0 && distRight < 75.0)
        {
            // detected enemy straight ahead
            moveForward();
        }
        else if (distLeft < 75.0)
        {
            // detected enemy to the left
            turnMoveLeft();
        }
        else if (distRight < 75.0)
        {
            // detected enemy to the right
            turnMoveRight();
        }
        else
        {
            // try to hunt for an enemy arbitrarily
            turnLeft();
        }
    }
}
