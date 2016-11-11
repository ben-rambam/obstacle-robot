// Lab 6b Mechatronics F16

// Name the pins used with the H-bridge
const int LeftMotorSpeedPin = 5;
const int LeftMotorControlA3Pin =  10;
const int LeftMotorControlA4Pin =  9;

const int RightMotorSpeedPin = 6;
const int RightMotorControlA1Pin =  8;
const int RightMotorControlA2Pin =  7;

int LeftLinePin = 0;
int CenterLinePin = 1;
int RightLinePin = 2;

int LeftMotorSpeed; // PWM values from 0 to 255
int RightMotorSpeed;

enum motorType {LEFT_MOTOR, RIGHT_MOTOR};
enum controlType {CW, CCW, STOP, COAST};

int lineSensors;
int prevState;
int currState;
int newState;

void goForward(int time)
{
  hBridge(LEFT_MOTOR, CCW);
  hBridge(RIGHT_MOTOR, CW);
  delay(time);
}

void goBack(int time)
{
  hBridge(LEFT_MOTOR, CW);
  hBridge(RIGHT_MOTOR, CCW);
  delay(time);
}

void turnLeft(int time)
{
  hBridge(LEFT_MOTOR, CW);
  hBridge(RIGHT_MOTOR, CW);
  delay(time);
}

void turnRight(int time)
{
  hBridge(LEFT_MOTOR, CCW);
  hBridge(RIGHT_MOTOR, CCW);
  delay(time);
}

void stop(int time)
{
  hBridge(LEFT_MOTOR, STOP);
  hBridge(RIGHT_MOTOR, STOP);
  delay(time);
}

void updateLineSensors()
{
  /*
  //if we were on the line, set the previous state bit
  lineSensors = prev & 7;
  if ( lineSensors )
  {
    lineSensors |= 8;
    lineSensors &= 8;
  }
  */

  //grab the new state
  //if new != curr
  //prev = curr
  //curr = new
  //if prev was on line
  //lineSensors.p = true;
  //else lineSensors.p = false;
    
  int RightOnLine = analogRead(RightLinePin) > 700 ? 1 : 0;
  int LeftOnLine = analogRead(LeftLinePin)> 700 ? 1 : 0;
  int CenterOnLine = analogRead(CenterLinePin)> 700 ? 1 : 0;

  newState = 0;
  newState |= LeftOnLine << 2;
  newState |= CenterOnLine << 1;
  newState |= RightOnLine;

  if ( newState != (currState & 7) )
  {
    prevState = currState;
    currState = newState;
    
    if (prevState & 7)
    {
      currState |= 8;
    }
    else
    {
      currState &= 7;
    }
  }

  return;
}

void spiral (long lastTime, long spiralTime)
{
  int left = 255*0.75;
  double K = 0.001;
  int right = left * (K*lastTime-5)/(K*lastTime +5);

  if ( right < 0 )
  {
    analogWrite(LeftMotorSpeedPin, left);
    analogWrite(RightMotorSpeedPin, -right);
    turnRight(spiralTime);
  }
  else
  {
    analogWrite(LeftMotorSpeedPin, left);
    analogWrite(RightMotorSpeedPin, right);
    goForward( spiralTime );
  }
  return;  
}

// Function initializes the H-bridge:  sets the digital pins as output
void hBridgeInit (void) {
  pinMode(LeftMotorSpeedPin, OUTPUT);      
  pinMode(LeftMotorControlA3Pin, OUTPUT);   
  pinMode(LeftMotorControlA4Pin, OUTPUT);  
  pinMode(RightMotorSpeedPin, OUTPUT);      
  pinMode(RightMotorControlA1Pin, OUTPUT);      
  pinMode(RightMotorControlA2Pin, OUTPUT);      
}

// Function to control the specified left or right motor
void hBridge (motorType motor, controlType control) {
  switch (motor) {
    case LEFT_MOTOR:
      switch (control) {
        case CW:
          // set the Left Motor CW
          digitalWrite(LeftMotorControlA3Pin, HIGH);   // sets the Left Motor CW
          digitalWrite(LeftMotorControlA4Pin, LOW);
          break;
        case CCW:
          // set the Left Motor CCW
          digitalWrite(LeftMotorControlA3Pin, LOW);   // sets the Left Motor CCW
          digitalWrite(LeftMotorControlA4Pin, HIGH);
          break;
        case STOP:
          // set the Left Motor stop
          analogWrite(LeftMotorSpeedPin, 255);        // set Left Motor enable high for braking
          digitalWrite(LeftMotorControlA3Pin, LOW);
          digitalWrite(LeftMotorControlA4Pin, LOW);
          break;
        case COAST:
          // set Left Motor Speed to 0 for coast.  Control pins are don't care
          analogWrite(LeftMotorSpeedPin, 0);
          break;
        default:
          // Should never get here
          break;
      }
      break;
    case RIGHT_MOTOR:
      switch (control) {
        case CW:
          // set the Left Motor CW
          digitalWrite(RightMotorControlA1Pin, LOW);   // sets the Left Motor CW
          digitalWrite(RightMotorControlA2Pin, HIGH);
          break;
        case CCW:
          // set the Left Motor CCW
          digitalWrite(RightMotorControlA1Pin, HIGH);   // sets the Left Motor CW
          digitalWrite(RightMotorControlA2Pin, LOW);
          break;
        case STOP:
          // set the Left Motor stop
          analogWrite(RightMotorSpeedPin, 255);        // set Left Motor enable high for braking
          digitalWrite(RightMotorControlA1Pin, LOW);   // sets the Left Motor CW
          digitalWrite(RightMotorControlA2Pin, LOW);
          break;
        case COAST:
          // set Left Motor Speed to 0 for coast.  Control pins are don't care
          analogWrite(RightMotorSpeedPin, 0);
          break;
        default:
          // Should never get here
          break;
      }
      break;
  }
}

// Setup runs once to initialize the system
void setup()
{
  Serial.begin(9600);
  hBridgeInit();

  LeftMotorSpeed = 128;    // Initial speed
  RightMotorSpeed = 128;  // Initial speed

  //hBridge(LEFT_MOTOR, CW);
  //hBridge(RIGHT_MOTOR, CW);


}

// After setup runs once, Loop runs continuously 
void loop()
{
  LeftMotorSpeed = 255*0.75;
  RightMotorSpeed = 255*0.75;

  int baseSpeed = 255*0.65;

  static int t;

 /* // set initial Left Motor Speed
  analogWrite(LeftMotorSpeedPin, LeftMotorSpeed);
  // set initial Right Motor Speed
  analogWrite(RightMotorSpeedPin, RightMotorSpeed);

  goForward(1000);
  stop(1000);
  turnRight(90);
  stop(1000);
  */
  //int lineSensors = updateLineSensors(lineSensors);

  int delayTime = 10;

  float shallow = 1.1;
  float hard = 1.5;

  Serial.print(prevState);
  Serial.print("  ");
  Serial.println(currState);

  long t0 = millis();
  while (currState == 0)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    spiral(millis()-t0, delayTime);
    updateLineSensors();
  }
  
  while (currState == 1)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    goForward(delayTime);
    
    updateLineSensors();
  }
  
  while (currState == 2)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    goForward(delayTime);
    updateLineSensors();
  }
  
  while (currState == 3)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed*shallow);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    goForward(delayTime);
    updateLineSensors();
  }
  
  while (currState == 4)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    goForward(delayTime);
    updateLineSensors();
  }
  while (currState == 5)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    turnLeft(delayTime);
    updateLineSensors();
  }
  while (currState == 6)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed*shallow);
    goForward(delayTime);
    updateLineSensors();
  }
  while (currState == 7)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    turnLeft(delayTime);
    updateLineSensors();
  }  
  while (currState == 8)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    turnLeft(delayTime);
    updateLineSensors();
  }  
  while (currState == 9)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed*hard);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    goForward(delayTime);
    updateLineSensors();
  }  
  while (currState == 10)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    goForward(delayTime);
    updateLineSensors();
  }  
  while (currState == 11)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed*shallow);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    goForward(delayTime);
    updateLineSensors();
  }  
  while (currState == 12)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed*hard);
    goForward(delayTime);
    updateLineSensors();
  }  
  while (currState == 13)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    turnLeft(delayTime);
    updateLineSensors();
  }  
  while (currState == 14)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed*shallow);
    goForward(delayTime);
    updateLineSensors();
  }  
  while (currState == 15)
  {
      Serial.print(prevState);
  Serial.print("  ");
    Serial.println(currState);
    analogWrite(LeftMotorSpeedPin, baseSpeed);
    analogWrite(RightMotorSpeedPin, baseSpeed);
    turnLeft(delayTime);
    updateLineSensors();
  }  
}



