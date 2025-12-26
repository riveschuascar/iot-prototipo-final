#include "WiFiConfig.h"

WiFiConfig::WiFiConfig() {}

bool WiFiConfig::connect(const char* apSsid, const char* apPassword) {
  WiFi.mode(WIFI_STA);
  
  // Timeouts configuration
  wm.setConfigPortalTimeout(60);   // 60 seconds for portal
  wm.setConnectTimeout(10);        // 5 seconds for connection attempt
  wm.setConnectRetries(3);         // 3 connection retries

  bool res = wm.autoConnect(apSsid, apPassword);
  return res;
}

bool WiFiConfig::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WiFiConfig::resetSettings() {
  wm.resetSettings();
}