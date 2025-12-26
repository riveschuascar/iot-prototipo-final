#include "GasSensor.h"

GasSensor::GasSensor(int pin) : pin(pin), calibrationValue(0) {}

void GasSensor::begin() {
  pinMode(pin, INPUT);
  
  // Calibración inicial (promedio de 10 lecturas)
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(100);
  }
  calibrationValue = sum / 10;
  
  Serial.println("Sensor de gas inicializado en pin " + String(pin));
  Serial.println("Valor de calibración: " + String(calibrationValue));
}

int GasSensor::read() {
  return analogRead(pin);
}

int GasSensor::readPPM() {
  int rawValue = analogRead(pin);
  // Conversión aproximada a PPM (ajustar según tu sensor)
  int ppm = map(rawValue, calibrationValue, 4095, 0, 1000);
  return max(0, ppm);
}