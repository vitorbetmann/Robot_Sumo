//TODO
// calibrate sensors based on initial values

//Edge Sensors
#include <QTRSensors.h>  //range = 0 to 1023. 0 = white, 1023 = black
#define leftIRSensor 0   //these numbers are arbitrary, could be anything
#define rightIRSensor 1
#define leftIRSensorPin A0  //They use analog pins
#define rightIRSensorPin A1
//Ultrasonic Sensor
#define triggerPin 2  //uses digital pins
#define echoPin 3
//Motors
#define ENALeft 5  //Enablers use PWM
#define ENBRight 10
#define IN1 7  //LEFT SIDE -- an IN's use digital only
#define IN2 6
#define IN3 9  //RIGHT SIDE
#define IN4 8
//other constants
#define soundSpeed 0.034  //cm per seconds

QTRSensors qtr;
const byte SensorCount = 2;
int sensorValues[SensorCount];
const byte arraySize = 2;  //works better with an even number
int arrayRight[arraySize];
int arrayLeft[arraySize];
float averageRight = 0;
float averageLeft = 0;
bool rightOnEdge;
bool leftOnEdge;
byte i = 0;
byte count = 0;

void setup() {
  Serial.begin(9600);  //comment out

  // configure the sensors
  qtr.setTypeAnalog();
  qtr.setSensorPins((const byte[]){ leftIRSensorPin, rightIRSensorPin }, SensorCount);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  for (byte j = 0; j < arraySize; j++) {
    qtr.read(sensorValues);
    arrayRight[j] = sensorValues[rightIRSensor];
    averageRight += arrayRight[j];
  }
  averageRight *= 0.5;  //multiply instead

  for (byte j = 0; j < arraySize; j++) {
    qtr.read(sensorValues);
    arrayLeft[j] = sensorValues[leftIRSensor];
    averageLeft += arrayLeft[j];
  }
  averageLeft *= 0.5;  //multiply instead

  //Enable Motors
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENALeft, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENBRight, OUTPUT);
  

  delay(5000);

  //first move here
  //firstMove();
  //while(true){}
}

void loop() {
  bool oppFound;

    qtr.read(sensorValues);  // read raw sensor values
    rightOnEdge = IRSensorOnEdge(rightIRSensor);
    leftOnEdge = IRSensorOnEdge(leftIRSensor);
    if (rightOnEdge) {
      getSet(rightIRSensor);
    } else if (leftOnEdge) {
      getSet(leftIRSensor);
    }

    //if not on edge, search for opp
    oppFound = search();
    if (oppFound) {
      attack();
    }
  

  i++;
}

void firstMove() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENBRight, 150);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENALeft, 150);
}

void getSet(byte sensorSide) {
  //Full stop
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENBRight, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENALeft, 0);
  delay(100);

  if (sensorSide == leftIRSensor) {
    //Reverse
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENBRight, 255); //--------------------------------------------------------------make both 255?
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENALeft, 255); //-------------------------------------------------------------make both 255?
    delay(400); //--------------------------------------------------------------------------------400 or 100?
  } else {
    //Reverse
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENBRight, 255); //------------------------------------------------------------make both 255?
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENALeft, 255); //-------------------------------------------------------------make both 255?
    delay(400); //-------------------------------------------------------------------------------400 or 100?
  }

  //Full stop
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENBRight, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENALeft, 0);
}

bool search() { //-----------------------------------------------------give search an argument? to see if it searches left and right?
  while (true) {

    if (oppDistance() <= 20) {
      return true;
    }
    qtr.read(sensorValues);  // read raw sensor values
    rightOnEdge = IRSensorOnEdge(rightIRSensor);
    leftOnEdge = IRSensorOnEdge(leftIRSensor);
    if (rightOnEdge) {
      return false;
    } else if (leftOnEdge) {
      return false;
    }

    //if opp not found yet, keep looking
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENALeft, 100); //--------------------------------------------------------------------------------change speeds?
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENBRight, 120); //--------------------------------------maybe the speed is good, maybe it covers too large a ring
    delay(50);
    count++;

    if (count == 90){
      count = 0;
      firstMove();
      delay(50);
    }

  }
}

void attack() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENALeft, 255);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENBRight, 255);
}

bool IRSensorOnEdge(byte sensorSide) {
  i = i % (arraySize);

  if (sensorSide == rightIRSensor) {
    averageRight -= arrayRight[i] * 0.5;  //multiply instead
    arrayRight[i] = sensorValues[sensorSide];
    averageRight += arrayRight[i] * 0.5;  //multiply instead
    return averageRight < 400;
  } else {
    averageLeft -= arrayLeft[i] * 0.5;  //multiply instead
    arrayLeft[i] = sensorValues[sensorSide];
    averageLeft += arrayLeft[i] * 0.5;  //multiply instead
    return averageLeft < 400;
  }
}
float oppDistance() {
  long duration;
  float distance;
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.5 * soundSpeed;
  return distance;
}
