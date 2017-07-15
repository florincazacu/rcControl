#include <SoftwareSerial.h> // TX RX software library for bluetooth

#include <Servo.h> // servo library 
Servo myServo; // servo name

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

// Motor1 output takes pins 6 and 9 instead of 3 and 5
int motorA1  = 6;  // Pin  6 of L298
int motorA2  = 9;  // Pin  9 of L298
int servoPwr = 3;  // Pin  3 of L298
int servoGnd = 5;  // Pin  5 of L298

int stream; // Received data from bluetooth
int servoPosition = 90; // Default servomotor position for straight car movement
int maxSpeed = 255; // Maximum engine speed
int currentSpeed = maxSpeed; // Current engine speed
int delayTime = 100;
int turnIncrement = 5;
int maxLeftAngle = 140;
int maxRightAngle = 50;


SoftwareSerial mySerial(bluetoothTx, bluetoothRx);

void setup() {
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(servoGnd, OUTPUT);
  pinMode(servoPwr, OUTPUT);
  analogWrite (servoGnd, 0);
  analogWrite(servoPwr, 255);
  myServo.attach(11); // Attach servo signal wire to pin 11
  myServo.write(servoPosition);
  Serial.print("Servo position: ");
  Serial.println(servoPosition);
  //Setup usb serial connection to computer
  Serial.begin(115200);
  //Setup Bluetooth serial connection to android
  mySerial.begin(115200);
}

void goForward() {
//  Serial.println("Forward");
  analogWrite(motorA1, currentSpeed);
  analogWrite(motorA2, 0);
//  Serial.print("Forward currentSpeed: ");
  Serial.println (currentSpeed);
//  Serial.print("motorA1: ");
//  Serial.print(analogRead(motorA1));
//  Serial.print(" motorA2: ");
//  Serial.println(analogRead(motorA2));
}

void goBack() {
//  Serial.println("Back");
  analogWrite(motorA1, 0);
  analogWrite(motorA2, currentSpeed);
//  Serial.print("Back currentSpeed: ");
  Serial.println (currentSpeed);
//  Serial.print("motorA1: ");
//  Serial.print(analogRead(motorA1));
//  Serial.print(" motorA2: ");
//  Serial.println(analogRead(motorA2));
}

unsigned long buttonPressTime;
long interval = 200;

void turnLeft() {
//  Serial.println("Left");
  if (millis() - buttonPressTime < interval){
  return;
  }
  if (servoPosition < maxLeftAngle) {
    servoPosition += turnIncrement;
    Serial.print("Servo position: ");
    Serial.println(servoPosition);
    myServo.write(servoPosition);
  } else {
    servoPosition = maxLeftAngle;
  }
  buttonPressTime = millis();
}

void turnRight() {
//  Serial.println("Right");
  if (millis() - buttonPressTime < interval){
  return;
  }
    if (servoPosition > maxRightAngle) {
    servoPosition -= turnIncrement;
    Serial.print("Servo position: ");
    Serial.println(servoPosition);
    myServo.write(servoPosition);
  } else {
    servoPosition = maxRightAngle;
  }
  buttonPressTime = millis();  
}

void stopCar() {
  analogWrite(motorA1, 0);  analogWrite(motorA2, 0);
}

void loop() {

  //Read from bluetooth and write to usb serial
  if (mySerial.available()) {
    stream = mySerial.read();
    if (stream != 83){
    Serial.print("stream received: ");
    Serial.println(stream);
    }
  }

  // Manipulate received data
  switch (stream) {
      Serial.print("stream: ");
      Serial.println(stream);
    case 'q':
    case STOP:
      stopCar();
      break;
    case '0':
    case '1':
    case '2':
    case '3':
//      currentSpeed = maxSpeed * 0.3;
      break;
    case '4':
    case '5':
    case '6':
//      currentSpeed = maxSpeed * 0.5;
      break;
    case '7':
    case '8':
    case '9':
//      currentSpeed = maxSpeed;
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
      goForward();
      turnLeft();
      break;
    case FORWARD_RIGHT:
      goForward();
      turnRight();
      break;
    case BACK_LEFT:
      goBack();
      turnLeft();
      break;
    case BACK_RIGHT:
      goBack();
      turnRight();
      break;
    case 88:
    case 120:
    myServo.write(90);
  }
}

