const int leftForward = 8;
const int leftBackward = 9;
const int rightForward = 10;
const int rightBackward = 11;

const int trigPin = 3;
const int echoPin = 2;

void setup() {
  Serial.begin(9600);
  // motor
  pinMode(leftForward, OUTPUT);
  pinMode(leftBackward, OUTPUT);
  pinMode(rightForward, OUTPUT);
  pinMode(rightBackward, OUTPUT);
  // sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

int sense() {
  digitalWrite(tr, HIGH);
  delayMicroseconds(10);
  digitalWrite(3, LOW);

  float duration_us = pulseIn(2, HIGH);

  // calculate the distance
  float distance_cm = 0.017 * duration_us;

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

}

// spins until an enemy is detected
void scan() {

}

void moveForward() {
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightBackward, LOW);
  digitalWrite(leftForward, HIGH);
  digitalWrite(rightForward, HIGH);
}

void moveBackward() {
  digitalWrite(leftForward, LOW);
  digitalWrite(rightForward, LOW);
  digitalWrite(leftBackward, HIGH);
  digitalWrite(rightBackward, HIGH);
}

void turnLeft() {
  // left motor backward right motor backward
  digitalWrite(leftForward, LOW);
  digitalWrite(rightBackward, LOW);
  digitalWrite(leftBackward, HIGH);
  digitalWrite(rightForward, HIGH);
}

void turnRight() {
  // left motor forward right motor forward 
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightForward, LOW);
  digitalWrite(leftForward, HIGH);
  digitalWrite(rightBackward, HIGH);
}

// move toward enemy
void charge() {

}

void loop() {
  scan();
  charge();
}
