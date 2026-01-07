#pragma once

#include <Arduino.h>

class FanController {
  public:
    FanController(int pin = 26);
    void begin();
    void turnOn();
    void turnOff();
    bool isOn();
    
  private:
    int pin;
    bool state;
};
