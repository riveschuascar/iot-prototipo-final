#include "WiFiConfig.h"
#include "AWSIoT.h"
#include "SmartThing.h"

WiFiConfig wifiConfig;
AWSIoT awsIoT;
SmartThing smartThing;

static uint8_t awsFailCount = 0;
const uint8_t MAX_AWS_FAILS = 3;
const uint32_t AWS_TIMEOUT = 20000; // 20 seconds

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
  if (!awsIoT.reconnectMQTT(AWS_TIMEOUT)) {
  Serial.println("AWS no disponible, borrando WiFi y reiniciando...");
  WiFi.disconnect(true, true);  // borra credenciales
  delay(1000);
  ESP.restart();
  }
}

void loop() {
  // Verificar WiFi
  if (!wifiConfig.isConnected()) {
    Serial.println("WiFi desconectado, reiniciando...");
    delay(1000);
    ESP.restart();
  }

  // Mantener AWS IoT
  if (!awsIoT.loop()) {
    awsFailCount++;
    Serial.printf("AWS fallo (%d/%d)\n", awsFailCount, MAX_AWS_FAILS);

    if (awsFailCount >= MAX_AWS_FAILS) {
      Serial.println("AWS no disponible, borrando WiFi...");
      WiFi.disconnect(true, true);
      delay(1000);
      ESP.restart();
    }
  } else {
    awsFailCount = 0;
  }

  // Actualizar lógica del dispositivo
  smartThing.loop();
}