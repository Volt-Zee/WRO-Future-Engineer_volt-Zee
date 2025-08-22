#include <Wire.h>
#include <Servo.h>

const int numSensors = 3; // 0=front,1=left,2=right
int distances[numSensors];

// Motor\ pins
const int ENA = 6;  // PWM speed
const int IN1 = 5;
const int IN2 = 4;

// Servo
Servo steeringServo;

const int SERVO_LEFT = 100;
const int SERVO_CENTER = 50;
const int SERVO_RIGHT = 0;

// Speeds (0-255)
const int speedF = 130;  // forward speed
const int speedB = 130;  // backward speed
const int speedT = 120;  // turning speed

int currentSteeringAngle = SERVO_CENTER;  // track current servo angle

void setup() {
  Serial.begin(9600);         // For debugging Nano receives data or not
  Wire.begin(8);              // I2C slave address
  Wire.onReceive(receiveEvent);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  steeringServo.attach(9);
  stopMotor();
  steeringServo.write(SERVO_CENTER);
}

void loop() {
  // Debug print distances
  Serial.print("Front: "); Serial.print(distances[0]);
  Serial.print(", Right: "); Serial.print(distances[1]);
  Serial.print(", Left: "); Serial.println(distances[2]);

  int frontDist = distances[0];
  int leftDist  = distances[1];
  int rightDist = distances[2];

if (frontDist > 0 && frontDist < 60) {
  // --- CORNER TURNING MODE ---
  if (leftDist > rightDist) {
    // Turn left for a longer corner
    steeringServo.write(85);   // Big left turn
    forward(speedT);
    delay(800);                 // Longer delay for big turn
  } 
  else {
    // Turn right for a longer corner
    steeringServo.write(10);   // Big right turn
    forward(speedT);
    delay(800);
  }
} 
else if (frontDist >= 30) {
  // --- STRAIGHT MIDDLE ADJUSTMENT ---
  if (leftDist > rightDist) {  
    // Slight right adjustment
    steeringServo.write(55);   
    forward(120);
    delay(30);
  } 
  else if (rightDist > leftDist ) {  
    // Slight left adjustment
    steeringServo.write(35);   // Small left
    forward(120);
    delay(30);
  }



  else {
    // Perfectly balanced — go straight
    steeringServo.write(45);   // Center
    forward(160);
  }
}
}







void receiveEvent(int howMany) {
  if (howMany < numSensors * 2) return;
  for (int i = 0; i < numSensors; i++) {
    int highByte = Wire.read();
    int lowByte = Wire.read();
    distances[i] = (highByte << 8) | lowByte;
  }
}

// Motor control functions
void forward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);
}

void backward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}
