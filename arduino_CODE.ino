#include <Servo.h>

Servo myservo;   // Servo for ultrasonic
Servo servo2;   // Servo for rain sensor

const int trigPin = 9;
const int echoPin = 10;
const int servoPin1 = 6;   // First servo (ultrasonic)
const int servoPin2 = 5;   // Second servo (rain)
const int rainSensorPin = 7;
const int buzzerPin = 8;   // Buzzer pin

long duration;
float distance;

bool movedUltrasonic = false;
bool movedRain = false;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(rainSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  myservo.attach(servoPin1);
  servo2.attach(servoPin2);

  myservo.write(90); // stop
  servo2.write(90);  // stop
}

void loop() {
  // ----- ULTRASONIC SENSOR SECTION -----
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 0 && distance <= 10 && !movedUltrasonic) {
    Serial.println("Object detected! Spinning servo1 forward.");

    myservo.write(80); // forward
    delay(500);
    myservo.write(90); // stop

    movedUltrasonic = true;
  }

  if (distance > 7 && movedUltrasonic) {
    Serial.println("Object gone. Waiting before reversing servo1.");

    delay(3000);
    myservo.write(100); // backward
    delay(605);
    myservo.write(90); // stop

    movedUltrasonic = false;
  }

  // ----- RAIN SENSOR SECTION -----
  int rainDetected = digitalRead(rainSensorPin);

  Serial.print("Rain Sensor: ");
  Serial.println(rainDetected == LOW ? "Rain Detected" : "No Rain");

  // If rain detected and servo2 hasn't moved yet
  if (rainDetected == LOW && !movedRain) {
    Serial.println("Rain detected! Spinning servo2 forward and playing Happy Birthday.");

    servo2.write(100); // counter-clockwise (forward)
    delay(750);
    servo2.write(90); // stop

    playHappyBirthday(); // play the song once

    movedRain = true; // mark moved
  }

  // If rain stopped and servo2 is moved, wait 3 seconds to confirm then move back
  if (rainDetected == HIGH && movedRain) {
    unsigned long dryStart = millis();
    bool stillDry = true;

    while (millis() - dryStart < 3000) {
      if (digitalRead(rainSensorPin) == LOW) {
        stillDry = false; // rain returned during waiting period
        break;
      }
      delay(50); // small delay to avoid busy looping
    }

    if (stillDry) {
      Serial.println("Rain stopped. Moving servo2 backward.");
      servo2.write(80); // clockwise (backward)
      delay(605);
      servo2.write(90); // stop

      movedRain = false; // reset flag
    }
  }

  delay(100);
}

// ----- Happy Birthday Song -----
void playHappyBirthday() {
  int melody[] = {
    262, 262, 294, 262, 349, 330, // Happy birthday to you
    262, 262, 294, 262, 392, 349, // Happy birthday to you
    262, 262, 523, 440, 349, 330, 294, // Happy birthday dear [Name]
    466, 466, 440, 349, 392, 349 // Happy birthday to you
  };

  int noteDurations[] = {
    300, 300, 600, 600, 600, 1200,
    300, 300, 600, 600, 600, 1200,
    300, 300, 600, 600, 600, 600, 1200,
    300, 300, 600, 600, 600, 1200
  };

  for (int i = 0; i < 26; i++) {
    tone(buzzerPin, melody[i], noteDurations[i]);
    delay(noteDurations[i] * 1.3);
    noTone(buzzerPin);
  }
}
