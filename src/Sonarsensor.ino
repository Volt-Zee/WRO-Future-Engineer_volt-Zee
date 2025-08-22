#include <Wire.h>
const int numSensors = 3;
const int trigPins[numSensors] = {13, 11,7}; 
const int echoPins[numSensors] = {12, 10,6};
// 13  12 left corner  // right corner 11 10 ,,,, front  

int distances[numSensors];

const int MAX_DISTANCE_CM = 400;
const int slaveAddress = 8;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Master

  for (int i = 0; i < numSensors; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    digitalWrite(trigPins[i], LOW);
  }
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    distances[i] = minDistance(trigPins[i], echoPins[i]);
  }

  Serial.print("Distances: ");
  for (int i = 0; i < numSensors; i++) {
    Serial.print(distances[i]);
    if (i < numSensors - 1) Serial.print(", ");
  }
  Serial.println();

  Wire.beginTransmission(slaveAddress);
  for (int i = 0; i < numSensors; i++) {
    Wire.write(highByte(distances[i]));
    Wire.write(lowByte(distances[i]));
  }
  Wire.endTransmission();

  delay(50); // More stable
}

int minDistance(int trig, int echo) {
  int d1 = ultrasonicRead(trig, echo);
  delayMicroseconds(500);
  int d2 = ultrasonicRead(trig, echo);

  if (d1 == -1) d1 = MAX_DISTANCE_CM;
  if (d2 == -1) d2 = MAX_DISTANCE_CM;

  return min(d1, d2);
}

int ultrasonicRead(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseInLong(echoPin, HIGH, 20000);
  if (duration == 0 || duration > 20000) return -1;

  return duration / 58;
}