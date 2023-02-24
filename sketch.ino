const int leftForward = 8;
const int leftBackward = 9;
const int rightForward = 10;
const int rightBackward = 11;

const int echoPinLeft = 2;
const int trigPinLeft = 3;
const int echoPinRight = 4;
const int trigPinRight = 5;

const float MAX_DIST = 30; // detection threshold

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(1)); // read random noise
  // motor
  pinMode(leftForward, OUTPUT);
  pinMode(leftBackward, OUTPUT);
  pinMode(rightForward, OUTPUT);
  pinMode(rightBackward, OUTPUT);
  // sensor
  pinMode(echoPinLeft, OUTPUT);
  pinMode(trigPinLeft, INPUT);
  pinMode(echoPinRight, OUTPUT);
  pinMode(trigPinRight, INPUT);
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
  digitalWrite(leftForward, LOW);
  digitalWrite(rightBackward, LOW);
  digitalWrite(leftBackward, HIGH);
  digitalWrite(rightForward, HIGH);
}

void turnAndMoveLeft() {
  // left motor backward right motor backward
  digitalWrite(leftForward, LOW);
  digitalWrite(rightBackward, LOW);
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightForward, HIGH);
}

void turnRight() {
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightForward, LOW);
  digitalWrite(leftForward, HIGH);
  digitalWrite(rightBackward, HIGH);
}

void turnAndMoveRight() {
  // left motor forward right motor forward 
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightForward, LOW);
  digitalWrite(leftForward, LOW);
  digitalWrite(rightBackward, HIGH);
}

float sense(int echoPin, int trigPin) {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float output = pulseIn(echoPin, HIGH);
  return 0.017 * output;
}

// spins until an enemy is detected
void scan() {
  float distLeft = sense(echoPinLeft, trigPinLeft);
  float distRight = sense(echoPinRight, trigPinRight);
  while(distLeft >= MAX_DIST || distRight >= MAX_DIST) {
    if(distLeft <= MAX_DIST) {
      turnAndMoveLeft();
    } else if(distRight <= MAX_DIST) {
      turnAndMoveRight();
    } else {
      // turn a random direction
      if(random(1)) {
        turnRight();
      } else {
        turnLeft();
      }
    }
    delayMicroseconds(100);
    distLeft = sense(echoPinLeft, trigPinLeft);
    distRight = sense(echoPinRight, trigPinRight);
  }
}

// move toward enemy
void charge() {
  Serial.println("Charging!");
  moveForward();
  delayMicroseconds(100);
}

void loop() {
  scan();
  charge();
  //delay(1000);
}
