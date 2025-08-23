#include <Wire.h>
#include <Servo.h>

// ---------------- Sensors ----------------
const int numSensors = 3; // 0=front,1=left,2=right
int distances[numSensors];

// ---------------- Motor Pins ----------------
const int ENA = 6;  // PWM speed (same pin as PWM in encoder code)
const int IN1 = 5;
const int IN2 = 4;

// ---------------- Servo ----------------
Servo steeringServo;
const int SERVO_LEFT = 100;
const int SERVO_CENTER = 50;
const int SERVO_RIGHT = 0;
int currentSteeringAngle = SERVO_CENTER;

// ---------------- Speeds ----------------
const int speedF = 130;  
const int speedB = 130;  
const int speedT = 120;  

// ---------------- Encoder ----------------
#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE
volatile long posi = 0; // motor position

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);

  // I2C setup
  Wire.begin(8);              
  Wire.onReceive(receiveEvent);

  // Motor setup
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Servo setup
  steeringServo.attach(9);
  stopMotor();
  steeringServo.write(SERVO_CENTER);

  // Encoder setup
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);

  Serial.println("System initialized with Encoder + Servo steering...");
}

// ---------------- Loop ----------------
void loop() {
  // Debug distances
  Serial.print("Front: "); Serial.print(distances[0]);
  Serial.print(", Right: "); Serial.print(distances[1]);
  Serial.print(", Left: "); Serial.println(distances[2]);

  // Debug encoder position
  Serial.print("Encoder Position: ");
  Serial.println(posi);

  int frontDist = distances[0];
  int leftDist  = distances[1];
  int rightDist = distances[2];

  // ---------------- Navigation Logic ----------------
  if (frontDist > 0 && frontDist < 60) {
    // --- CORNER TURNING MODE ---
    if (leftDist > rightDist) {
      steeringServo.write(90);   // Big left turn
      forward(speedT);
      delay(700);
    } else {
      steeringServo.write(10);   // Big right turn
      forward(speedT);
      delay(700);
    }
  } 
  else if (frontDist >= 30) {
    // --- STRAIGHT MIDDLE ADJUSTMENT ---
    if (leftDist > rightDist + 5) {  
      steeringServo.write(55);   // Slight right
      forward(120);
      delay(30);
    } 
    else if (rightDist > leftDist + 5) {  
      steeringServo.write(35);   // Slight left
      forward(120);
      delay(30);
    }
    else if (rightDist < 25) {
      steeringServo.write(60);
      forward(120);
      delay(50);
    }
    else if (leftDist < 25) {
      steeringServo.write(30);
      forward(120);
      delay(50);
    }
    else {
      // Balanced — go straight
      steeringServo.write(45);
      forward(160);
    }
  }

  // ---------------- Encoder-based Stop ----------------
  if (posi > 5000 && posi < 5500) {
    stopMotor();
  }

  delay(100);
}

// ---------------- I2C Receive Event ----------------
void receiveEvent(int howMany) {
  if (howMany < numSensors * 2) return;
  for (int i = 0; i < numSensors; i++) {
    int highByte = Wire.read();
    int lowByte = Wire.read();
    distances[i] = (highByte << 8) | lowByte;
  }
}

// ---------------- Encoder ISR ----------------
void readEncoder() {
  int b = digitalRead(ENCB);
  if (b > 0) posi++;
  else posi--;
}

// ---------------- Motor Control ----------------
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
