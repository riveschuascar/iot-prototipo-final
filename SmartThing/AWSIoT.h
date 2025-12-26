#pragma once

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <functional>

#define THING_NAME "esp32-maqueta"
#define MQTT_MAX_PACKET_SIZE 4096

typedef std::function<void(const char* topic, const char* payload)> MessageCallback;

class AWSIoT {
public:
  AWSIoT();
  void begin();
  void loop();
  void publishState(const char* jsonState);
  void getState();
  bool isConnected();
  void setMessageCallback(MessageCallback callback);
  void reconnectMQTT();

private:
  WiFiClientSecure wifiClient;
  PubSubClient client;
  MessageCallback messageCallback;

  char mqttServer[100];
  char clientId[50];
  char publishTopic[100];
  char getTopic[100];
  char publishPayload[MQTT_MAX_PACKET_SIZE];
  char* subscribeTopic[5];

  void setupCertificates();
  void setupTopics();
  static void callback(char* topic, byte* payload, unsigned int length);
  static AWSIoT* instance;
};