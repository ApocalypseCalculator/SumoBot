const int leftForward = 8;
const int leftBackward = 9;
const int rightForward = 10;
const int rightBackward = 11;

const int trigPin = 3;
const int echoPin = 2;

const float diameter = 30;

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

float sense() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float output = pulseIn(echoPin, HIGH);
  return 0.017 * output;
}

// spins until an enemy is detected
void scan() {
  float dist = sense();
  while(dist >= diameter) {
    Serial.print("Scanning distance of ");
    Serial.print(dist, 3);
    Serial.println();
    turnRight();
    delayMicroseconds(1000000000);
    dist = sense();
  }
  Serial.print("Found enemy: ");
  Serial.print(dist, 3);
  Serial.println();
}

// move toward enemy
void charge() {
  Serial.println("Charging!");
  moveForward();
  delayMicroseconds(1000000000);
}

void loop() {
  scan();
  charge();
  delay(1000);
}
