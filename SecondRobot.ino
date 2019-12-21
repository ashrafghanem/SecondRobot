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
#define leapTime 200

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
  analogWrite(ENA, 100);
  analogWrite(ENB, 140);

  pinMode(led, OUTPUT);
  Serial.begin(9600);
  HC12.begin(9600);
  digitalWrite(led, HIGH);

  delay(1000);
}


void loop() {
  while (HC12.available()) {       // If HC-12 has data
    Serial.write(HC12.read());     // Send the data to Serial monitor
  }
  return;

  readSensors();

  if (leftFarReading < 200 && rightFarReading < 200 &&
      (leftCenterReading > 200 || rightCenterReading > 200) ) {
    straight();
  }
  else {
    leftHandWall();
  }
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

void leftHandWall() {
  if ( leftFarReading > 200 && rightFarReading > 200) { // T or cross or destination
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(leapTime);
    readSensors();

    if (leftFarReading > 200 || rightFarReading > 200) {
      done();
    }
    if (leftFarReading < 200 && rightFarReading < 200) {
      turnLeft();
    }
  }

  if (leftFarReading > 200) { // if you can turn left then turn left
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(leapTime);
    readSensors();

    if (leftFarReading < 200 && rightFarReading < 200) {
      turnLeft();
    }
    else {
      done();
    }
  }

  if (rightFarReading > 200) { // straight or right
    digitalWrite(leftMotor1, HIGH);
    digitalWrite(leftMotor2, LOW);
    digitalWrite(rightMotor1, HIGH);
    digitalWrite(rightMotor2, LOW);
    delay(30);
    readSensors();

    if (leftFarReading > 200) {
      delay(leapTime - 30);
      readSensors();

      if (rightFarReading > 200 && leftFarReading > 200) {
        done();
      }
      else {
        turnLeft();
        return;
      }
    }
    delay(leapTime - 30);
    readSensors();
    if (leftFarReading < 200 && leftCenterReading < 200 &&
        rightCenterReading < 200 && rightFarReading < 200) {
      turnRight();
      return;
    }
    path[pathLength] = 'S';
    pathLength++;
    if (path[pathLength - 2] == 'B') {
      shortPath();
    }
    straight();
  }
  readSensors();
  if (leftFarReading < 200 && leftCenterReading < 200 && rightCenterReading < 200
      && rightFarReading < 200) {
    turnAround();
  }
}

void done() {
  digitalWrite(leftMotor1, LOW);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, LOW);
  digitalWrite(rightMotor2, LOW);
  replaystage = 1;
  path[pathLength] = 'D';
  pathLength++;

  while (convertValue(digitalRead(leftFarSensor)) > 200) {
    digitalWrite(led, LOW);
    delay(150);
    digitalWrite(led, HIGH);
    delay(150);
  }
  delay(500);
  replay();
}

void turnLeft() {
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

  if (replaystage == 0) {
    path[pathLength] = 'L';
    pathLength++;
    if (path[pathLength - 2] == 'B') {
      shortPath();
    }
  }
}

void turnRight() {
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

  if (replaystage == 0) {
    path[pathLength] = 'R';
    pathLength++;
    if (path[pathLength - 2] == 'B') {
      shortPath();
    }
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

void turnAround() {
  digitalWrite(leftMotor1, HIGH);
  digitalWrite(leftMotor2, LOW);
  digitalWrite(rightMotor1, HIGH);
  digitalWrite(rightMotor2, LOW);
  delay(150);

  while (convertValue(digitalRead(leftCenterSensor)) < 200) {
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

  path[pathLength] = 'B';
  pathLength++;
  straight();
}

void shortPath() {
  int shortDone = 0;
  if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'R') {
    pathLength -= 3;
    path[pathLength] = 'B';
    shortDone = 1;
  }

  if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'S' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'R';
    shortDone = 1;
  }

  if (path[pathLength - 3] == 'R' && path[pathLength - 1] == 'L' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'B';
    shortDone = 1;
  }


  if (path[pathLength - 3] == 'S' && path[pathLength - 1] == 'L' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'R';
    shortDone = 1;
  }

  if (path[pathLength - 3] == 'S' && path[pathLength - 1] == 'S' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'B';
    shortDone = 1;
  }
  if (path[pathLength - 3] == 'L' && path[pathLength - 1] == 'L' && shortDone == 0) {
    pathLength -= 3;
    path[pathLength] = 'S';
    shortDone = 1;
  }

  path[pathLength + 1] = 'D';
  path[pathLength + 2] = 'D';
  pathLength++;
}

void printPath() {
  Serial.println("+++++++++++++++++");
  int x;
  while (x <= pathLength) {
    Serial.println(path[x]);
    x++;
  }
  Serial.println("+++++++++++++++++");
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
