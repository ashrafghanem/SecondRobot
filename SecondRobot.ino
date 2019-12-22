#include <SoftwareSerial.h>

SoftwareSerial HC12(A0, 2); // TX, RX of HC-12

#define leftCenterSensor   9
//#define leftNearSensor     A4
#define leftFarSensor      4
#define rightCenterSensor  10
//#define rightNearSensor    A1
#define rightFarSensor     11

int leftCenterReading;
//int leftNearReading;
int leftFarReading;
int rightCenterReading;
//int rightNearReading;
int rightFarReading;

int replaystage;
#define leapTime 150

int ENB = 5;
#define leftMotor1  6
#define leftMotor2  8

int ENA = 3;
#define rightMotor1 12
#define rightMotor2 7

#define led 13

char path[30] = {};
int pathLength;
int readLength;

void setup() {
  pinMode(leftCenterSensor, INPUT);
  //  pinMode(leftNearSensor, INPUT);
  pinMode(leftFarSensor, INPUT);
  pinMode(rightCenterSensor, INPUT);
  //  pinMode(rightNearSensor, INPUT);
  pinMode(rightFarSensor, INPUT);

  pinMode(leftMotor1, OUTPUT);
  pinMode(leftMotor2, OUTPUT);
  pinMode(rightMotor1, OUTPUT);
  pinMode(rightMotor2, OUTPUT);

  // HC12 serial
  pinMode(A0, INPUT);
  pinMode(2, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  analogWrite(ENA, 95);
  analogWrite(ENB, 135);

  pinMode(led, OUTPUT);
  Serial.begin(9600);
  HC12.begin(9600);
  digitalWrite(led, HIGH);

  delay(1000);
}

void loop() {
  analogWrite(ENA, 95);
  analogWrite(ENB, 135);
  while (HC12.available()) {            // If HC-12 has data
    path[pathLength] = HC12.read();     // read path from HC-12
    pathLength++;
  }
  if (path[pathLength - 1] != 'D') {
    return;
  }

  replaystage = 1;
  int flushTimes = 5;
  while (flushTimes < 5) {
    digitalWrite(led, LOW);
    delay(150);
    digitalWrite(led, HIGH);
    delay(150);
    flushTimes++;
  }
  delay(500);

  replay();
}

int convertValue(int real) {
  if (real == 1) {
    return 250;
  }
  else {
    return 100;
  }
}

void readSensors() {
  leftCenterReading  = convertValue(digitalRead(leftCenterSensor));
  //  leftNearReading    = convertValue(digitalRead(leftNearSensor));
  leftFarReading     = convertValue(digitalRead(leftFarSensor));
  rightCenterReading = convertValue(digitalRead(rightCenterSensor));
  //  rightNearReading   = convertValue(digitalRead(rightNearSensor));
  rightFarReading    = convertValue(digitalRead(rightFarSensor));

  //   serial printing below for debugging purposes
  //   Serial.print("leftFarReading: ");
  //   Serial.println(leftFarReading);
  //
  //   Serial.print("leftCenterReading: ");
  //   Serial.println(leftCenterReading);
  //
  ////   Serial.print("leftNearReading: ");
  ////   Serial.println(leftNearReading);
  //
  //   Serial.print("rightCenterReading: ");
  //   Serial.println(rightCenterReading);
  //
  ////   Serial.print("rightNearReading: ");
  ////   Serial.println(rightNearReading);
  //
  //   Serial.print("rightFarReading: ");
  //   Serial.println(rightFarReading);
  //
  //   delay(2500);
}

void turnLeft() {
  analogWrite(ENA, 105);
  analogWrite(ENB, 145);
  while (convertValue(digitalRead(rightCenterSensor)) > 200 || convertValue(digitalRead(leftCenterSensor)) > 200) {
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(2);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }

  while (convertValue(digitalRead(rightCenterSensor)) < 200) {
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, HIGH);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(2);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }
}

void turnRight() {
  analogWrite(ENA, 105);
  analogWrite(ENB, 145);
  while (convertValue(digitalRead(rightCenterSensor)) > 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
    delay(2);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }
  while (convertValue(digitalRead(rightCenterSensor)) < 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
    delay(2);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }
  while (convertValue(digitalRead(leftCenterSensor)) < 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, HIGH);
    delay(2);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(1);
  }
}

void straight() {
  if ( convertValue(digitalRead(leftCenterSensor)) < 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(1);
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, LOW);
    digitalWrite(rightMotor2, LOW);
    delay(5);
    return;
  }
  if (convertValue(digitalRead(rightCenterSensor)) < 200) {
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(1);
    digitalWrite(leftMotor1, LOW);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(5);
    return;
  }

  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  delay(4);
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
  delay(1);
}

void replay() {
  readSensors();
  if (leftFarReading < 200 && rightFarReading < 200) {
    straight();
  }
  else {
    if (path[readLength] == 'D') {
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(100);
      digitalWrite(leftMotor1, LOW);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, LOW);
      digitalWrite(rightMotor2, LOW);
      endMotion();
    }
    if (path[readLength] == 'L') {
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(leapTime);
      turnLeft();
    }
    if (path[readLength] == 'R') {
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(leapTime);
      turnRight();
    }
    if (path[readLength] == 'S') {
      digitalWrite(leftMotor1, HIGH);
      digitalWrite(leftMotor2, LOW);
      digitalWrite(rightMotor1, HIGH);
      digitalWrite(rightMotor2, LOW);
      delay(leapTime);
      straight();
    }
    readLength++;
  }
  replay();
}

void endMotion() {
  digitalWrite(led, LOW);
  delay(500);
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
  delay(500);
  endMotion();
}
