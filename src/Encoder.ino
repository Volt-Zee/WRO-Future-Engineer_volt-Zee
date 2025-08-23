#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE
#define PWM 6
#define IN2 4
#define IN1 5

volatile long posi = 0; // Use long to accommodate larger position values

void setup() {
Serial.begin(9600);
pinMode(ENCA, INPUT);
pinMode(ENCB, INPUT);
attachInterrupt(digitalPinToInterrupt(ENCA), readEncoder, RISING);

pinMode(PWM, OUTPUT);
pinMode(IN1, OUTPUT);
pinMode(IN2, OUTPUT);

Serial.println("Car position calculation using encoder pulses...");
}

void loop() {
// Run the motor continuously
digitalWrite(IN1,1);digitalWrite(IN2,0);analogWrite(PWM,200);
// Print the current position
Serial.print("Current position: ");
Serial.println(posi);

// Add a delay to avoid overwhelming the serial monitor
delay(100);

if(posi>5000 && posi<5500){
digitalWrite(IN1,0);
digitalWrite(IN2,0);
analogWrite(PWM,0);
}
}



void readEncoder() {
int b = digitalRead(ENCB);
if (b > 0) {
posi++;
} else {
posi--;
}
}