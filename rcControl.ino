#include <TimedAction.h>

#include <SoftwareSerial.h> // TX RX software library for bluetooth

#include <Servo.h> // servo library 

Servo frontServo; // front servo
Servo rearServo; // rear servo

#define FORWARD 70
#define BACK 66
#define LEFT 76
#define RIGHT 82
#define FORWARD_LEFT 71
#define FORWARD_RIGHT 73
#define BACK_LEFT 72
#define BACK_RIGHT 74
#define STOP 83

int bluetoothTx = 4; // bluetooth tx to 4 pin
int bluetoothRx = 2; // bluetooth rx to 2 pin

int motorA1  = 6;  // Pin  6 of L298
int motorA2  = 9;  // Pin  9 of L298

int receivedStream; // Received data from bluetooth
int frontServoPosition = 90; // Default servomotor position for straight car movement
int rearServoPosition = 90; // Default servomotor position for straight car movement
int maxSpeed = 255; // Maximum engine speed
int currentSpeed = maxSpeed; // Current engine speed
int frontTurnIncrement = 5;
int rearTurnIncrement = 5;
int maxFrontLeftAngle = 130;
int maxFrontRightAngle = 50;

int maxRearLeftAngle = 110;
int maxRearRightAngle = 70;

int turnDelay = 50;
boolean turn = true;

void canTurn() {
  if (turn == false) {
    turn = true;
  }
}

SoftwareSerial mySerial(bluetoothTx, bluetoothRx);
TimedAction timedAction = TimedAction(turnDelay, canTurn);

void setup() {
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  frontServo.attach(11); // Attach servo signal wire to pin 11
  rearServo.attach(10); // Attach servo signal wire to pin 10
  frontServo.write(frontServoPosition);
  rearServo.write(rearServoPosition);

  //Setup usb serial connection to computer
  Serial.begin(115200);

  //Setup Bluetooth serial connection to android
  mySerial.begin(115200);
}

void goForward() {
  analogWrite(motorA1, currentSpeed);
  analogWrite(motorA2, 0);
}

void goBack() {
  analogWrite(motorA1, 0);
  analogWrite(motorA2, currentSpeed);
}

void turnLeftRear() {
  if (rearServoPosition > maxRearRightAngle && rearServoPosition <= maxRearLeftAngle) {
    rearServoPosition -= rearTurnIncrement;
    Serial.print("Rear Servo position: ");
    Serial.println(rearServoPosition);
    rearServo.write(rearServoPosition);
  }
}

void turnLeft() {
  if (frontServoPosition < maxFrontLeftAngle && frontServoPosition >= maxFrontRightAngle && turn == 1) {
    frontServoPosition += frontTurnIncrement;
    turnLeftRear();
    frontServo.write(frontServoPosition);
  }
  turn = false;
}

void turnRightRear() {
  if (rearServoPosition < maxRearLeftAngle && rearServoPosition >= maxRearRightAngle) {
    rearServoPosition += rearTurnIncrement;
    rearServo.write(rearServoPosition);
  }
}

void turnRight() {
  if (frontServoPosition > maxFrontRightAngle && frontServoPosition <= maxFrontLeftAngle && turn == 1) {
    frontServoPosition -= frontTurnIncrement;
    turnRightRear();
    frontServo.write(frontServoPosition);
  }
  turn = false;
}

void goForwardLeft() {
  analogWrite(motorA1, currentSpeed);
  analogWrite(motorA2, 0);
  turnLeft();
}

void goForwardRight() {
  analogWrite(motorA1, currentSpeed);
  analogWrite(motorA2, 0);
  turnRight();
}

void goBackLeft() {
  analogWrite(motorA1, 0);
  analogWrite(motorA2, currentSpeed);
  turnLeft();
}

void goBackRight() {
  analogWrite(motorA1, 0);
  analogWrite(motorA2, currentSpeed);
  turnRight();
}

void stopCar() {
  analogWrite(motorA1, 0);  analogWrite(motorA2, 0);
}

void loop() {

  timedAction.check();

  //Read from bluetooth and write to usb serial
  if (mySerial.available()) {
    receivedStream = mySerial.read();
    if (receivedStream != 83) {
      Serial.print("receivedStream received: ");
      Serial.println(receivedStream);
    }
  }

  // Manipulate received data
  switch (receivedStream) {
      Serial.print("receivedStream: ");
      Serial.println(receivedStream);
    case 'q':
    case STOP:
      stopCar();
      break;
    case '0':
    case '1':
    case '2':
    case '3':
            currentSpeed = maxSpeed * 0.3;
      break;
    case '4':
    case '5':
    case '6':
            currentSpeed = maxSpeed * 0.5;
      break;
    case '7':
    case '8':
    case '9':
            currentSpeed = maxSpeed;
      break;
    case FORWARD:
      goForward();
      break;
    case BACK:
      goBack();
      break;
    case LEFT:
      turnLeft();
      break;
    case RIGHT:
      turnRight();
      break;
    case FORWARD_LEFT:
      goForwardLeft();
      break;
    case FORWARD_RIGHT:
      goForwardRight();
      break;
    case BACK_LEFT:
      goBackLeft();
      break;
    case BACK_RIGHT:
      goBackRight();
      break;
    case 88:
    case 120:
      frontServoPosition = 90;
      rearServoPosition = 90;
      frontServo.write(frontServoPosition);
      rearServo.write(rearServoPosition);
  }
}

