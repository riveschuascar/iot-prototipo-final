#include "FanController.h"

FanController::FanController(int pin) : pin(pin), state(false) {}

void FanController::begin() {
  pinMode(pin, OUTPUT);
  turnOff();
  Serial.println("Ventilador inicializado en pin " + String(pin));
}

void FanController::turnOn() {
  digitalWrite(pin, HIGH);
  state = true;
  Serial.println("Ventilador encendido");
}

void FanController::turnOff() {
  digitalWrite(pin, LOW);
  state = false;
  Serial.println("Ventilador apagado");
}

bool FanController::isOn() {
  return state;
}