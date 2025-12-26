#include "WiFiConfig.h"

WiFiConfig::WiFiConfig() {
  // Constructor
}

bool WiFiConfig::connect(const char* apSsid, const char* apPassword) {
  WiFi.mode(WIFI_STA);
  
  // Configurar timeouts
  wm.setConfigPortalTimeout(60);   // 60 segundos para el portal cautivo
  wm.setConnectTimeout(10);        // 5 segundos para intentar conectar
  wm.setConnectRetries(3);         // 3 reintentos

  Serial.println("Intentando conectar a WiFi");
  bool res = wm.autoConnect(apSsid, apPassword);
  return res;
}

bool WiFiConfig::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void WiFiConfig::resetSettings() {
  wm.resetSettings();
  Serial.println("Credenciales WiFi borradas");
}