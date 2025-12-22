#include "GateServo.h"

GateServo::GateServo(uint8_t pin, uint8_t openAngle, uint8_t closeAngle)
  : pin(pin),
    openAngle(openAngle),
    closeAngle(closeAngle),
    state(closed) {}

void GateServo::begin() {
  gate.attach(pin);
  gate.write(closeAngle);
}

void GateServo::openGate() {
  if (state == open) return; // If already open, do nothing

  gate.write(openAngle);

  state = open;
}

void GateServo::closeGate() {
  if (state == closed) return; // If already closed, do nothing

  gate.write(closeAngle);

  state = closed;
}

GateServo::GateState GateServo::getState() const {
  return state;
}

void GateServo::setOpenAngle(uint8_t angle) {
  openAngle = constrain(angle, 0, 180); // Limit the angle beetwen 0 and 180
}
