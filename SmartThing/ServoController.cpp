#include "ServoController.h"

ServoController::ServoController(int pin) : pin(pin), currentAngle(0) {}

void ServoController::begin() {
  servo.attach(pin);
  servo.write(0);
  Serial.println("Servo inicializado en pin " + String(pin));
}

void ServoController::setAngle(int angle) {
  if (angle >= 0 && angle <= 180) {
    servo.write(angle);
    currentAngle = angle;
  }
}

int ServoController::getAngle() {
  return currentAngle;
}