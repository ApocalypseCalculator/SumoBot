const int leftForward = 8;
const int leftBackward = 9;
const int rightForward = 10;
const int rightBackward = 11;
const int sensor_trig = 3;
const int sensor_echo = 2;
void setup()
{
  pinMode(leftForward, OUTPUT);
  pinMode(leftBackward, OUTPUT);
  pinMode(rightForward, OUTPUT);
  pinMode(rightBackward, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(sensor_trig, OUTPUT); // trig
  pinMode(sensor_echo, INPUT);
  Serial.begin(9600);
  Serial.print("BEGIN");
}
void loop()
{
  digitalWrite(leftForward, HIGH);
  digitalWrite(leftBackward, LOW);
  digitalWrite(rightForward, HIGH);
  digitalWrite(rightBackward, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(sensor_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensor_trig, LOW);
  float duration_us = pulseIn(sensor_echo, HIGH);
  float distance_cm = 0.017 * duration_us;
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  delay(5);
}