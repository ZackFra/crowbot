#include <Arduino.h>

#define IR_RECEIVE_PIN  35  
#define LEFT_MOTOR_BACKWARDS    12  
#define LEFT_MOTOR_FORWARDS    13
#define RIGHT_MOTOR_BACKWARDS   14
#define RIGHT_MOTOR_FORWARDS   15

#define LEFT_IR_SENSOR 36
#define RIGHT_IR_SENSOR 39

#define BASE_SPEED 60

#define KP 0.2
#define KI 0.02
#define KD 0.002
#define KS 0.05 // how much you slow down while steering happens

#define MIN_LEFT 98
#define MAX_LEFT 963
#define MIN_RIGHT 77
#define MAX_RIGHT 2395

float integral = 0;
float lastError = 0;

int readIRSensor(int sensorPin) {
  int sensorValue = analogRead(sensorPin);
  if (sensorValue > 1000) {
    return 1;
  } else {
    return 0;
  }
}

void stop() {
  analogWrite(LEFT_MOTOR_FORWARDS, 0);
  analogWrite(RIGHT_MOTOR_FORWARDS, 0);
  analogWrite(LEFT_MOTOR_BACKWARDS, 0);
  analogWrite(RIGHT_MOTOR_BACKWARDS, 0);
}

void setWheel(int forwardPin, int backwardPin, int speed) {
  speed = constrain(speed, -255, 255);
  if(speed >= 0) {
    analogWrite(backwardPin, 0);
    analogWrite(forwardPin, speed);
  } else {
    analogWrite(forwardPin, 0);
    analogWrite(backwardPin, -speed);
  }
}

void moveForward() {
  stop();
  analogWrite(LEFT_MOTOR_FORWARDS, BASE_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARDS, BASE_SPEED);
}

void moveBackward() {
  stop();
  analogWrite(LEFT_MOTOR_BACKWARDS, BASE_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARDS, BASE_SPEED);
}

void moveLeft() {
  stop();
  analogWrite(LEFT_MOTOR_FORWARDS, BASE_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARDS, BASE_SPEED);
}

void moveRight() {
  stop();
  analogWrite(LEFT_MOTOR_BACKWARDS, BASE_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARDS, BASE_SPEED);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LEFT_IR_SENSOR, INPUT_PULLUP);
  pinMode(RIGHT_IR_SENSOR, INPUT_PULLUP);

  stop();
}

void bangBang() {

  int leftIRSensor = readIRSensor(LEFT_IR_SENSOR);
  int rightIRSensor = readIRSensor(RIGHT_IR_SENSOR);

  if (leftIRSensor == 1 && rightIRSensor == 1) {
    moveForward();
  } else if (leftIRSensor == 1 && rightIRSensor == 0) {
    moveLeft();
  } else if (leftIRSensor == 0 && rightIRSensor == 1) {
    moveRight();
  } else {
    stop();
  }

  delay(10);
}

int maxRight = 0;
int maxLeft = 0;
int minRight = 5000;
int minLeft = 5000;
void calibrate() {
  int leftSensorValue = analogRead(LEFT_IR_SENSOR);
  int rightSensorValue = analogRead(RIGHT_IR_SENSOR);

  maxRight = max(maxRight, rightSensorValue);
  maxLeft = max(maxLeft, leftSensorValue);
  minRight = min(minRight, rightSensorValue);
  minLeft = min(minLeft, leftSensorValue);

  Serial.print("Max Right: ");
  Serial.println(maxRight);
  Serial.print("Max Left: ");
  Serial.println(maxLeft);
  Serial.print("Min Right: ");
  Serial.println(minRight);
  Serial.print("Min Left: ");
  Serial.println(minLeft);

  int leftConstrained = constrain(leftSensorValue, MIN_LEFT, MAX_LEFT);
  int rightConstrained = constrain(rightSensorValue, MIN_RIGHT, MAX_RIGHT);
  int leftNormalized = map(leftConstrained, MIN_LEFT, MAX_LEFT, 0, 1000);
  int rightNormalized = map(rightConstrained, MIN_RIGHT, MAX_RIGHT, 0, 1000);

  Serial.print("Left Normalized: ");
  Serial.println(leftNormalized);
  Serial.print("Right Normalized: ");
  Serial.println(rightNormalized);
  delay(1000);
}

void pid() {
  int leftIRSensor = analogRead(LEFT_IR_SENSOR);
  int rightIRSensor = analogRead(RIGHT_IR_SENSOR);

  int leftConstrained = constrain(leftIRSensor, MIN_LEFT, MAX_LEFT);
  int rightConstrained = constrain(rightIRSensor, MIN_RIGHT, MAX_RIGHT);
  int leftNormalized = map(leftConstrained, MIN_LEFT, MAX_LEFT, 0, 1000);
  int rightNormalized = map(rightConstrained, MIN_RIGHT, MAX_RIGHT, 0, 1000);

  int error;
  bool isLost = (leftNormalized < 100 && rightNormalized < 100);
  if (isLost) {
    error = lastError;
  } else {
    error = leftNormalized - rightNormalized;
  }
  int P = error * KP;
  integral = integral + error * 0.01;
  int I = integral * KI;
  int D = KD * (error - lastError) / 0.01;
  int u = P + I + D;

  int speed = BASE_SPEED - KS * abs(error);
  if(speed < 0) {
    speed = 0;
  }

  lastError = error;

  setWheel(LEFT_MOTOR_FORWARDS, LEFT_MOTOR_BACKWARDS, speed + u);
  setWheel(RIGHT_MOTOR_FORWARDS, RIGHT_MOTOR_BACKWARDS, speed - u);

  delay(10);

}

void loop() {
  pid();
}
