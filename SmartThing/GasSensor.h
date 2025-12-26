#pragma once

#include <Arduino.h>

class GasSensor {
  public:
    GasSensor(int pin = 34);
    void begin();
    int read();
    int readPPM();
    
  private:
    int pin;
    int calibrationValue;
};
