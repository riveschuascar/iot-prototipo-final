#include "SmartThing.h"

SmartThing::SmartThing()
  : lastGasCheck(0),          // Now
    gasCheckInterval(10000),  // 10 seconds default
    currentServoAngle(0),     // Closed
    fanEnabled(false),        // Off
    autoMode(false),          // Manual mode
    gasThresholdServo(200),   // PPM
    gasThresholdFan(400)      // PPM
{}

void SmartThing::begin() {
  Serial.println("Inicializando Smart Thing...");

  servo.begin();
  gasSensor.begin();
  fan.begin();

  servo.setAngle(0);
  fan.turnOff();

  Serial.println("Smart Thing inicializado");
}

void SmartThing::setStateCallback(StateCallback callback) {
  stateCallback = callback;
}

void SmartThing::loop() {
  // Verificar nivel de gas periódicamente
  if (millis() - lastGasCheck >= gasCheckInterval) {
    checkGasLevel();
    // Control automático si está habilitado
    if (autoMode) {
      autoControl();
    }
    lastGasCheck = millis();
  }
}

void SmartThing::checkGasLevel() {
  int gasLevel = gasSensor.read();

  StaticJsonDocument<200> doc;
  doc["state"]["reported"]["gasLevel"] = gasLevel;

  if (gasLevel <= 300) {
    doc["state"]["reported"]["alert"] = "SEGURO";
  }
  else if (gasLevel <= 1000) {
    doc["state"]["reported"]["alert"] = "GAS_DETECTADO";
  }
  else if (gasLevel <= 5000) {
    doc["state"]["reported"]["alert"] = "ADVERTENCIA";
  }
  else if (gasLevel <= 10000) {
    doc["state"]["reported"]["alert"] = "PELIGRO";
  }
  else if (gasLevel <= 50000) {
    doc["state"]["reported"]["alert"] = "RIESGO_EXPLOSION";
  }
  else {
    doc["state"]["reported"]["alert"] = "CRITICO";
  }

  char buffer[256];
  serializeJson(doc, buffer);

  // If publish callback funtion is declared, publish readings
  if (stateCallback) {
    stateCallback(buffer);
  }
}

void SmartThing::autoControl() {
  int gasLevel = gasSensor.read();

  // Si el gas supera el umbral, encender ventilador
  if (gasLevel > gasThresholdFan) {
    fan.turnOn();
    fanEnabled = true;
    Serial.println("Ventilador encendido automáticamente");
  }
  // Si el gas baja, apagar ventilador
  else if (gasLevel < (gasThresholdFan - 50)) {
    fan.turnOff();
    fanEnabled = false;
    Serial.println("Ventilador apagado automáticamente");
  }

  // Si el gas supera el umbral, abrir el servo
  if (gasLevel > gasThresholdServo) {
    servo.setAngle(0);
    currentServoAngle = 0;
    Serial.println("Servo abierto automáticamente");
  }
  // Si el gas baja, cerrar el servo
  else if (gasLevel < (gasThresholdServo - 50)) {
    servo.setAngle(90);
    currentServoAngle = 90;
    Serial.println("Servo cerrado automáticamente");
  }
  publishCurrentState();
}

void SmartThing::handleAWSMessage(const char* topic, const char* payload) {
  Serial.printf("\n[SmartThing] Procesando mensaje de: %s\n", topic);
  Serial.printf("Payload recibido (%d bytes): %s\n", strlen(payload), payload);

  // Verificar tamaño del payload
  size_t payloadSize = strlen(payload);
  if (payloadSize >= 512) {
    Serial.printf("[WARNING] Payload muy grande (%d bytes), puede estar truncado\n", payloadSize);
    Serial.println("[WARNING] Considere aumentar MQTT_MAX_PACKET_SIZE o usar buffer dinamico");
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.printf("[ERROR] Error parseando JSON: %s\n", error.c_str());

    // Debug detallado de errores de deserializacion
    switch (error.code()) {
      case DeserializationError::EmptyInput:
        Serial.println("[DEBUG] Payload vacio o nulo");
        break;
      case DeserializationError::IncompleteInput:
        Serial.println("[DEBUG] JSON incompleto - posiblemente truncado por buffer pequeno");
        Serial.printf("[DEBUG] Tamanio del payload: %d bytes\n", payloadSize);
        Serial.println("[DEBUG] Solucion: Aumentar MQTT_MAX_PACKET_SIZE en AWSIoT.h");
        break;
      case DeserializationError::InvalidInput:
        Serial.println("[DEBUG] JSON invalido - caracteres no esperados");
        Serial.println("[DEBUG] Primeros 100 caracteres del payload:");
        Serial.println(String(payload).substring(0, 100));
        break;
      case DeserializationError::NoMemory:
        Serial.println("[DEBUG] Sin memoria suficiente para parsear JSON");
        Serial.println("[DEBUG] Solucion: Aumentar el tamano de StaticJsonDocument");
        break;
      case DeserializationError::TooDeep:
        Serial.println("[DEBUG] JSON demasiado anidado (max profundidad excedida)");
        break;
      default:
        Serial.printf("[DEBUG] Error desconocido: %d\n", error.code());
        break;
    }
    return;
  }

  // Verificar si es respuesta de /shadow/get/accepted (estado completo)
  if (strstr(topic, "/shadow/get/accepted") != NULL) {
    Serial.println("[INFO] Recibiendo estado completo del shadow");

    JsonObject state = doc["state"];

    if (state.containsKey("desired") && !state["desired"].isNull()) {
      Serial.println("[INFO] Aplicando estado deseado inicial");
      handleDesiredState(state["desired"]);
    } else if (state.containsKey("reported") && !state["reported"].isNull()) {
      Serial.println("[INFO] No hay estado deseado, aplicando estado reportado inicial");
      handleDesiredState(state["reported"]);
    } else {
      Serial.println("[WARNING] Shadow vacio - no hay estado desired ni reported");
      Serial.println("[INFO] Publicando estado actual del dispositivo");
      publishCurrentState();
    }
    return;
  }

  // Verificar si es respuesta de /shadow/get/rejected
  if (strstr(topic, "/shadow/get/rejected") != NULL) {
    Serial.println("[ERROR] Solicitud de estado rechazada por AWS IoT");
    if (doc.containsKey("code")) {
      Serial.printf("[ERROR] Codigo de error: %d\n", doc["code"].as<int>());
    }
    if (doc.containsKey("message")) {
      Serial.printf("[ERROR] Mensaje: %s\n", doc["message"].as<const char*>());
    }
    return;
  }

  // Verificar si es update/rejected
  if (strstr(topic, "/shadow/update/rejected") != NULL) {
    Serial.println("[ERROR] Actualizacion de estado rechazada por AWS IoT");
    if (doc.containsKey("code")) {
      Serial.printf("[ERROR] Codigo de error: %d\n", doc["code"].as<int>());
    }
    if (doc.containsKey("message")) {
      Serial.printf("[ERROR] Mensaje: %s\n", doc["message"].as<const char*>());
    }
    return;
  }

  // Verificar si hay estado deseado (update/accepted o update/delta)
  if (doc.containsKey("state") && doc["state"].containsKey("desired")) {
    JsonObject desired = doc["state"]["desired"];
    if (!desired.isNull() && desired.size() > 0) {
      Serial.println("[INFO] Aplicando cambios desde desired state");
      handleDesiredState(desired);
    } else {
      Serial.println("[DEBUG] Estado deseado vacio, ignorando");
    }
  } else {
    Serial.println("[WARNING] Mensaje sin estado deseado reconocible");
    Serial.println("[DEBUG] Topics esperados: /shadow/get/accepted, /shadow/update/accepted, /shadow/update/delta");
  }
}

void SmartThing::handleDesiredState(JsonObject desired) {
  bool stateChanged = false;

  // Control del servo
  if (desired.containsKey("servo")) {
    int angle = desired["servo"];
    if (angle >= 0 && angle <= 180) {
      servo.setAngle(angle);
      currentServoAngle = angle;
      stateChanged = true;
      Serial.printf("Servo ajustado a: %d grados\n", angle);
    }
  }

  // Control del ventilador
  if (desired.containsKey("fan")) {
    bool enable = desired["fan"];
    if (enable) {
      fan.turnOn();
      fanEnabled = true;
      Serial.println("Ventilador encendido");
    } else {
      fan.turnOff();
      fanEnabled = false;
      Serial.println("Ventilador apagado");
    }
    stateChanged = true;
  }

  // Intervalo de lecturas
  if (desired.containsKey("sensorInterval")) {
    gasCheckInterval = desired["sensorInterval"];
    Serial.printf("Intervalo de lecturas ajustado a: %d segundos\n", gasCheckInterval);
    stateChanged = true;
  }

  // Modo automático
  if (desired.containsKey("autoMode")) {
    autoMode = desired["autoMode"];
    Serial.printf("Modo automático: %s\n", autoMode ? "ON" : "OFF");
    stateChanged = true;
  }

  // Umbral de gas ventilador
  if (desired.containsKey("gasThresholdFan")) {
    gasThresholdFan = desired["gasThresholdFan"];
    Serial.printf("Umbral de gas ventilador ajustado a: %d ppm\n", gasThresholdFan);
    stateChanged = true;
  }

  // Umbral de gas servo
  if (desired.containsKey("gasThresholdServo")) {
    gasThresholdServo = desired["gasThresholdServo"];
    Serial.printf("Umbral de gas servo ajustado a: %d ppm\n", gasThresholdServo);
    stateChanged = true;
  }

  // Publicar estado actualizado
  if (stateChanged) {
    publishCurrentState();
  }
}

void SmartThing::publishCurrentState() {
  StaticJsonDocument<512> doc;

  int gasLevel = gasSensor.read();

  doc["state"]["reported"]["servo"] = currentServoAngle;
  doc["state"]["reported"]["fan"] = fanEnabled;
  doc["state"]["reported"]["gasLevel"] = gasLevel;
  doc["state"]["reported"]["sensorInterval"] = gasCheckInterval;
  doc["state"]["reported"]["autoMode"] = autoMode;
  doc["state"]["reported"]["gasThresholdServo"] = gasThresholdServo;
  doc["state"]["reported"]["gasThresholdFan"] = gasThresholdFan;

  char buffer[512];
  serializeJson(doc, buffer);

  if (stateCallback) {
    stateCallback(buffer);
  }
}