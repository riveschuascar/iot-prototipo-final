#pragma once

#include <ESP32Servo.h>

class ServoController {
  public:
    ServoController(int pin = 25);
    void begin();
    void setAngle(int angle);
    int getAngle();
    
  private:
    Servo servo;
    int pin;
    int currentAngle;
};
