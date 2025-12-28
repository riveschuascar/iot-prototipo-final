#include "GasSensor.h"

GasSensor::GasSensor(int pin) : pin(pin), calibrationValue(0) {}

void GasSensor::begin() {
  pinMode(pin, INPUT);
  
  // Calibración inicial (promedio de 50 lecturas)
  long sum = 0;
  for (int i = 0; i < 50; i++) {
    sum += analogRead(pin);
    delay(100);
  }
  calibrationValue = sum / 50;
  
  Serial.println("Sensor de gas inicializado en pin " + String(pin));
  Serial.println("Valor de calibración: " + String(calibrationValue));
}

int GasSensor::read() {
  return analogRead(pin);
}

int GasSensor::readPPM() {
  int rawValue = analogRead(pin);
  // Conversión aproximada a PPM
  int ppm = map(rawValue, calibrationValue, 4095, 0, 1000);
  return max(0, ppm);
}