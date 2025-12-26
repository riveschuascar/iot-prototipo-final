#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include "ServoController.h"
#include "GasSensor.h"
#include "FanController.h"

typedef std::function<void(const char* jsonState)> StateCallback;

class SmartThing {
public:
  SmartThing();
  void begin();
  void loop();
  void handleAWSMessage(const char* topic, const char* payload);
  void setStateCallback(StateCallback callback);

private:
  ServoController servo;
  GasSensor gasSensor;
  FanController fan;

  StateCallback stateCallback;

  unsigned long lastGasCheck;
  unsigned long gasCheckInterval;

  int currentServoAngle;
  bool fanEnabled;
  bool autoMode;
  int gasThreshold;

  void publishCurrentState();
  void handleDesiredState(JsonObject desired);
  void checkGasLevel();
  void autoControlFan();
};
