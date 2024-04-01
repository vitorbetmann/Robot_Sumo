

//Edge Sensors
#define leftIRPin A0   //They use analog pins.
#define rightIRPin A1  //
#define leftIR 0       //Side values are arbitrary.
#define rightIR 1      //
//Ultrasonic Sensors
#define triggerPin 4  //Trigger uses digital pin.
#define echoPin 5     //Echo uses digital pin.
//Motors
#define ENA 6   //Left Side -- Enablers use PWM pins.
#define IN1 8   //IN's use digital pins.
#define IN2 7   //
#define IN3 9   //Right Side.
#define IN4 10  //
#define ENB 11  //
//Other constants
#define INITIAL_DELAY 5000       //Milliseconds *Standard delay for Mini Sumobot matches is 5000 ms.
#define MAX_PULSE_DURATION 5600  //Microseconds.
#define SOUND_SPEED 0.0343       //Cm per microsecond.
#define COLOUR_THRESHOLD 400     //Value that represent the threshold between what's considered white and black.

#define FORWARD 1                  //Directions values are arbitrary, but add to/represent HIGH/true (1), or LOW/false (0).
#define BACKWARDS 0                //More on that on the setMotors() and setMotorsSpin() methods.
#define CLOCKWISE 1                //
#define COUNTERCLOCKWISE 0         //
int standardRotation = CLOCKWISE;  //Set this direction based on the opponent. If the opp moves CW, go CCW and vice-versa.

//Variables
//*Explanation for the use of arrays in the updateUSValues() and updateIRValues() methods
const byte IRArraySize = 2;     //Amount of values needed to get an average.
int leftIRArray[IRArraySize];   //Stores values read by the left IR Sensor.
int rightIRArray[IRArraySize];  //Stores values read by the right IR Sensor.
float leftIRAverage = 0;        //Stores an average of the values in the left IR array.
float rightIRAverage = 0;       //Stores an average of the values in the right IR array.

const byte USArraySize = 2;  //Amount of values needed to perform the sum.
float USArray[USArraySize];  //Stores values read by the US Sensor.
float USValuesSum = 0;       //Stores the sum of all values in the US array.

byte i = 0;  //Iterates over the elements of the IR arrays.
byte j = 0;  //Iterates over the elements of the US array.

//SETUP & LOOP BELOW
//////////////////////////////////////////////////////////////////////////////
void setup() {
  //Start countdown for initial delay.
  long matchStart = millis();

  //Enable Edge Sensors && Populate IR arrays and get averages
  for (byte k = 0; k < IRArraySize; k++) {
    leftIRArray[k] = analogRead(leftIR);
    rightIRArray[k] = analogRead(rightIR);

    leftIRAverage += leftIRArray[k];
    rightIRAverage += rightIRArray[k];
  }
  leftIRAverage /= IRArraySize;
  rightIRAverage /= IRArraySize;

  //Enable Ultrasonic Sensors
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //Populate US array and get sum of values
  for (byte k = 0; k < USArraySize; k++) {
    USArray[k] = getOppDistance();
    USValuesSum += USArray[k];
  }

  //Enable Motors
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  //Wait for match to begin
  while (millis() - matchStart <= INITIAL_DELAY) {
    // do nothing
  }

  //First Moves -- Opponent dependand, enable with care.
  // attackFromSide();
  // blindAttack();
  // extraDelay(1000);
}

void loop() {
  if (oppFound()) {
    attack();
  } else if (leftIROnEdge()) {
    reverse();
    standardRotation = CLOCKWISE;
  } else if (rightIROnEdge()) {
    reverse();
    standardRotation = COUNTERCLOCKWISE;
  } else {
    revolutionSearch(200, 0);
  }
  updateIRValues();
}

//METHODS BELOW
///////////////////////////////////////////////////////////////////////////////

//Turns the bot to one side and move it out of the opponent's way before they reach it. Then, it turns to the opposite direction to find the opponent and push them from the side.
//* No need to change standardRotation after enabling this method!!! If opp moves CW, keep going CCW and vice-versa.
//* The bot will move on the opposite direction in relation to standardRotation at first, but this method already takes into consideration that we do not want the opp facing our bot after evading.
void attackFromSide() {
  standardRotation = !standardRotation;
  while (oppFound()) {
    revolutionSearch(255, 0);
  }
  standardRotation = !standardRotation;
  move(FORWARD, 255, 255);
  long timer = millis();  //Start of a timer.
  while (!oppFound() && (millis() - timer < 300)) {  // 300 is arbitrary, needs more testing
    //Do nothing
  }
  timer = millis();
  spinSearch(255);
  while (!oppFound() && (millis() - timer < 500)) {  // 500 is arbitraty, needs more testing
    //Do nothing
  }
}

//The purpose of this method is to just ram against opponents that are hard to find.
//For example, if they use some cloth to absorb our ultrasonic waves or they're hollow and our signal just goes through them.
void blindAttack() {
  while (true) {
    attack();
  }
}

//Just calls delay(delayAmount). It's sometimes beneficial to wait a bit longer before moving.
//This is the most risky of first moves. USE WITH CARE!
void extraDelay(int delayAmount) {
  delay(delayAmount);
}

//Moves forward at max speed.
void attack() {
  move(FORWARD, 255, 255);
}

//Moves back at max speed.
void reverse() {
  move(BACKWARDS, 255, 255);
}

//Stops the bot.
void stop() {
  move(FORWARD, 0, 0);
}

//Bot performs a revolution around a point in the arena while searching for the opponent.
//If only one speed is zero, it revolves around one wheel. If both speeds are same, it moves FORWARD at that speed.
//If highSpeed < lowSpeed, its revolution direction is the opposite of standardRotation.
void revolutionSearch(int highSpeed, int lowSpeed) {
  if (standardRotation == CLOCKWISE) {
    move(FORWARD, highSpeed, lowSpeed);
  } else {
    move(FORWARD, lowSpeed, highSpeed);
  }
}

//Bot spins around its own axis while searching for the opponent.
void spinSearch(int spinSpeed) {
  spin(standardRotation, spinSpeed);
}

//Rreturns true if there's an object within ~45 cm of distance.
//(distance depends on MAX_PULSE_DURATION).
bool oppFound() {
  updateUSValues();
  return USValuesSum > 0.1;
}

//Returns true if left IR sensor's average is below COLOUR_THRESHOLD.
//(i.e. if it sees white)
bool leftIROnEdge() {
  return leftIRAverage < COLOUR_THRESHOLD;
}

//Returns true if right IR sensor's average is below COLOUR_THRESHOLD.
//(i.e. if it sees white)
bool rightIROnEdge() {
  return rightIRAverage < COLOUR_THRESHOLD;
}

//Moves bot FORWARD or BACKWARDS if both wheels at same speed, and performs revolution when wheels at different speeds.
void move(int direction, int leftSpeed, int rightSpeed) {
  setMotors(direction);
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

//Spins bot around its own axis in CLOCKWISE or COUNTERCLOCKWISE direction, with both wheels at the same speed but opposite direction.
void spin(int rotationDirection, int speed) {
  setMotorsSpin(rotationDirection);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

//Sets motors for the bot to move FORWARD or BACKWARDS.
void setMotors(int direction) {
  //When IN'S 1-4 are set to {0, 1, 0, 1} (respectively), the bot moves forward
  //When set to {1, 0, 1, 0}, the bot moves backwards
  digitalWrite(IN1, 1 - direction);
  digitalWrite(IN2, 0 + direction);
  digitalWrite(IN3, 1 - direction);
  digitalWrite(IN4, 0 + direction);
}

//Sets motors for the bot to spin CLOCKWISE or COUNTERCLOCKWISE.
void setMotorsSpin(int rotationDirection) {
  //When IN'S 1-4 are set to {0, 1, 1, 0} (respectively), the bot spins clockwise
  //When set to {1, 0, 0, 1}, the bot spins counterclockwise
  digitalWrite(IN1, 1 - rotationDirection);
  digitalWrite(IN2, 0 + rotationDirection);
  digitalWrite(IN3, 0 + rotationDirection);
  digitalWrite(IN4, 1 - rotationDirection);
}

//Updates the sum of distance values returned by getOppDistance().
//At any point this sum is greater than 0, consider there is an object within ~45 cm from the bot.
void updateUSValues() {
  //#NOTE 1
  //The reason for having a sum of values is because during testing we
  //discovered that every other value returned by the Ultrasonic Sensor
  //was a zero (more on that on the getOppDistance() method), even if the
  //opponent was still in front of it. This was causing our robot to move
  //towards the opponent in an arch, rather than in a straight line (because
  //it was shifting from attack mode to searching for the opp with every
  //other update of the main loop), so we'd often lose sight of the opp,
  //which gave them time to attack us first.
  //By making our attack rely on a sum of 2 or more values, we guarantee that
  //as long as there's a positive value in the array, we will consider the
  //opponent as found and attack. This ensures our code is not affected by
  //noise, and that made rushing toward the opp more reliable

  //#NOTE 2
  //Depending on the size of the array, taking the sum of all elements
  //every time the main code loops could take too long.
  //So what this method does is just replace the oldest value in the sum
  //with a new value, updating, then, USValuesSum

  j = j % (USArraySize);

  USValuesSum -= USArray[j];
  USArray[j] = getOppDistance();
  USValuesSum += USArray[j];

  j++;
}

//Updates the averages of values read by each QTR sensor.
//At any point this average of either is less than COLOUR_THRESHOLD, consider bot is at an edge.
void updateIRValues() {
  //#NOTE 1
  //The reason for having an average of values for each IR sensor is to
  //avoid fooling our code with noise. When using only 1 value, we noticed
  //that our robot would spontaneously reverse for no apparent reason, as if
  //it were on the edge, even if it wasn't. By relying the decision of reversing
  //onto an average of values, we now need more data and, therefore, take more
  //time to react to being on the edge, but we neutralize the problems of having
  //the robot act based on noise. Furthermroe, updating both averages takes less
  //than 1 ms, so it's a worth trade off

  //#NOTE 2
  //Depending on the size of the array, adding all elements in it and
  //taking the average every time the main code loops could take too long.
  //So what this method does is just replace the oldest value used to get
  //the average of each side with a new value, updating, then, both left
  //and right side averages

  i = i % (IRArraySize);

  leftIRAverage -= leftIRArray[i] / IRArraySize;
  leftIRArray[i] = analogRead(leftIR);
  leftIRAverage += leftIRArray[i] / IRArraySize;

  rightIRAverage -= rightIRArray[i] / IRArraySize;
  rightIRArray[i] = analogRead(rightIR);
  rightIRAverage += rightIRArray[i] / IRArraySize;

  i++;
}

//Returns the distance, in cm, of an object to the Ultrasonic sensor.
//Returns 0 if nothing in sensor range before MAX_PULSE_DURATION microseconds go by.
float getOppDistance() {
  unsigned long pulseDuration; //Time it takes to emmit a pulse and receive it back in microsseconds.
  float distance; //Distance, in cm, to an object.
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pulseDuration = pulseIn(echoPin, HIGH, MAX_PULSE_DURATION);
  distance = pulseDuration * SOUND_SPEED * 0.5;
  return distance;
}
