#include "WiFiConfig.h"

WiFiConfig::WiFiConfig() {}

bool WiFiConfig::connect(const char* apSsid, const char* apPassword) {
  WiFi.mode(WIFI_STA);
  
  // Timeouts configuration
  wm.setConfigPortalTimeout(60);   // 60 seconds for portal
  wm.setConnectTimeout(10);        // 5 seconds for connection attempt
  wm.setConnectRetries(3);         // 3 connection attempts

  // Tries to connect to saved WiFi and opens the Portal in case connection fails
  bool res = wm.autoConnect(apSsid, apPassword);
  return res;
}

bool WiFiConfig::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WiFiConfig::resetSettings() {
  // Erase saved WiFi from memory
  wm.resetSettings();
}