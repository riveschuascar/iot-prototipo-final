#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>

class GateServo {
public:
  enum GateState : uint8_t {
    closed,
    open
  };

  GateServo(uint8_t pin, uint8_t openAngle, uint8_t closeAngle);

  void begin();
  void openGate();
  void closeGate();
  GateState getState() const;
  void setOpenAngle(uint8_t angle);
  
private:
  uint8_t pin;
  uint8_t openAngle;
  uint8_t closeAngle;
  GateState state;
  Servo gate;
};
