#include "AWSIoT.h"
#include "certificates.h"

AWSIoT* AWSIoT::instance = nullptr;

AWSIoT::AWSIoT() : client(wifiClient) {
  instance = this;
  strcpy(mqttServer, "a1n3h8klbo6gqj-ats.iot.us-west-1.amazonaws.com");
  strcpy(clientId, "ESmarthP-32");
}

void AWSIoT::begin() {
  setupCertificates();
  setupTopics();
  
  client.setServer(mqttServer, 8883);
  client.setCallback(AWSIoT::callback);
  client.setBufferSize(MQTT_MAX_PACKET_SIZE);
  
  Serial.println("AWS IoT inicializado");
}

void AWSIoT::setupCertificates() {
  wifiClient.setCACert(ROOT_CA);
  wifiClient.setCertificate(CERTIFICATE);
  wifiClient.setPrivateKey(PRIVATE_KEY);
  
  Serial.println("Certificados configurados desde PROGMEM");
}

void AWSIoT::setupTopics() {
  sprintf(publishTopic, "$aws/things/%s/shadow/update", THING_NAME);
  sprintf(getTopic, "$aws/things/%s/shadow/get", THING_NAME);
  
  subscribeTopic[0] = new char[100];
  subscribeTopic[1] = new char[100];
  subscribeTopic[2] = new char[100];
  subscribeTopic[3] = new char[100];
  subscribeTopic[4] = new char[100];
  
  sprintf(subscribeTopic[0], "$aws/things/%s/shadow/get/accepted", THING_NAME);
  sprintf(subscribeTopic[1], "$aws/things/%s/shadow/get/rejected", THING_NAME);
  sprintf(subscribeTopic[2], "$aws/things/%s/shadow/update/delta", THING_NAME);
  sprintf(subscribeTopic[3], "$aws/things/%s/shadow/update/accepted", THING_NAME);
  sprintf(subscribeTopic[4], "$aws/things/%s/shadow/update/rejected", THING_NAME);
}

void AWSIoT::setMessageCallback(MessageCallback callback) {
  messageCallback = callback;
}

void AWSIoT::callback(char* topic, byte* payload, unsigned int length) {
  if (instance != nullptr && instance->messageCallback) {
    char buffer[MQTT_MAX_PACKET_SIZE];
    strncpy(buffer, (const char*)payload, length);
    buffer[length] = '\0';
    
    instance->messageCallback(topic, buffer);
  }
}

void AWSIoT::publishState(const char* jsonState) {
  if (client.connected()) {
    client.publish(publishTopic, jsonState);
    Serial.printf("Published to update: %s\n", jsonState);
  } else {
    Serial.println("No se pudo publicar: cliente MQTT no conectado");
  }
}

void AWSIoT::getState() {
  if (client.connected()) {
    if (client.publish(getTopic, "{}")) {
      Serial.println("Solicitando estado");
      Serial.printf("Published to: %s\n", getTopic);
      client.loop();
    } else {
      Serial.println("No se pudo solicitar estado");
    }
  } else {
    Serial.println("Cliente MQTT no conectado, no se puede solicitar estado");
  }
}

void AWSIoT::reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando a AWS IoT... ");
    
    if (client.connect(clientId)) {
      Serial.println("conectado!");
      
      // Suscribirse a todos los topics
      for (int i = 0; i < 5; i++) {
        bool subscribed = client.subscribe(subscribeTopic[i]);
        if (subscribed) {
          Serial.printf("Suscrito a: %s\n", subscribeTopic[i]);
        } else {
          Serial.printf("Error suscribiendo a: %s\n", subscribeTopic[i]);
        }
        delay(100);
      }
      
      Serial.println("\nTodas las suscripciones completadas");
      
      // Solicitar el estado actual del shadow automáticamente
      delay(500); // Pequeño delay para asegurar que las suscripciones estén activas
      getState();
      
    } else {
      Serial.printf("falló, rc=%d. Reintentando en 5s\n", client.state());
      delay(5000);
    }
  }
}

void AWSIoT::loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}

bool AWSIoT::isConnected() {
  return client.connected();
}