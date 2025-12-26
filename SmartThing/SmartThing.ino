#include "WiFiConfig.h"
#include "AWSIoT.h"
#include "SmartThing.h"

WiFiConfig wifiConfig;
AWSIoT awsIoT;
SmartThing smartThing;

void setup() {
  Serial.begin(115200);
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  Smart Thing ESP32 - AWS IoT Core      ║");
  Serial.println("╚════════════════════════════════════════╝");

  // Configurar WiFi
  Serial.println("Configurando WiFi...");
  if (!wifiConfig.connect("Conf-AWS-IoT", "12345678")) {
    Serial.println("No se pudo conectar a WiFi, reiniciando en 3 segundos");
    delay(3000);
    ESP.restart();
  }

  Serial.print("WiFi conectado, ");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Inicializar componentes
  Serial.println("Inicializando componentes");
  smartThing.begin();

  // Configurar callbacks
  Serial.println("Configurando AWS IoT");
  
  awsIoT.setMessageCallback([](const char* topic, const char* payload) {
    smartThing.handleAWSMessage(topic, payload);
  });

  smartThing.setStateCallback([](const char* jsonState) {
    awsIoT.publishState(jsonState);
  });

  // Inicializar AWS IoT (Solo define los topics y certificados)
  awsIoT.begin();
  
  // Conectar a AWS y suscribirse a los topics
  awsIoT.reconnectMQTT();
}

void loop() {
  // Verificar WiFi
  if (!wifiConfig.isConnected()) {
    Serial.println("WiFi desconectado, reiniciando...");
    delay(1000);
    ESP.restart();
  }

  // Mantener AWS IoT activo
  awsIoT.loop();
  
  // Actualizar lógica del dispositivo
  smartThing.loop();
}