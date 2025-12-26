#pragma once

#include <WiFi.h>
#include <WiFiManager.h>

class WiFiConfig {
private:
  WiFiManager wm;
public:
  WiFiConfig();
  bool connect(const char* apSsid, const char* apPassword);
  bool isConnected();
  void resetSettings();
};
