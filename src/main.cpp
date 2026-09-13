#include <Arduino.h>

#define IR_RECEIVE_PIN  35  
#define LEFT_MOTOR_BACKWARDS    12  
#define LEFT_MOTOR_FORWARDS    13
#define RIGHT_MOTOR_BACKWARDS   14
#define RIGHT_MOTOR_FORWARDS   15

#define LEFT_IR_SENSOR 36
#define RIGHT_IR_SENSOR 39

#define HALF_SPEED 60

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

void moveForward() {
  stop();
  analogWrite(LEFT_MOTOR_FORWARDS, HALF_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARDS, HALF_SPEED);
}

void moveBackward() {
  stop();
  analogWrite(LEFT_MOTOR_BACKWARDS, HALF_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARDS, HALF_SPEED);
}

void moveLeft() {
  stop();
  analogWrite(LEFT_MOTOR_FORWARDS, HALF_SPEED);
  analogWrite(RIGHT_MOTOR_BACKWARDS, HALF_SPEED);
}

void moveRight() {
  stop();
  analogWrite(LEFT_MOTOR_BACKWARDS, HALF_SPEED);
  analogWrite(RIGHT_MOTOR_FORWARDS, HALF_SPEED);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LEFT_IR_SENSOR, INPUT_PULLUP);
  pinMode(RIGHT_IR_SENSOR, INPUT_PULLUP);

  stop();
}

void loop() {

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

  delay(50);
}
