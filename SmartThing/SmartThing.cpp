#include "SmartThing.h"

SmartThing::SmartThing()
  : lastGasCheck(0),
    gasCheckInterval(10000), // 10 seconds
    currentServoAngle(0),
    fanEnabled(false),
    autoMode(false),
    gasThreshold(300) {
}

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
    lastGasCheck = millis();
  }

  // Control automático si está habilitado
  if (autoMode) {
    autoControlFan();
  }
}

void SmartThing::checkGasLevel() {
  int gasLevel = gasSensor.read();

  // Si el gas supera el umbral, publicar alerta
  if (gasLevel > gasThreshold) {
    Serial.printf("ALERTA: Nivel de gas alto: %d ppm\n", gasLevel);

    StaticJsonDocument<200> doc;
    doc["state"]["reported"]["gasLevel"] = gasLevel;
    doc["state"]["reported"]["alert"] = "high_gas";
    doc["clientToken"] = "smartThingClient";

    char buffer[256];
    serializeJson(doc, buffer);

    if (stateCallback) {
      stateCallback(buffer);
    }
  }
}

void SmartThing::autoControlFan() {
  int gasLevel = gasSensor.read();

  // Si el gas supera el umbral, encender ventilador
  if (gasLevel > gasThreshold && !fanEnabled) {
    fan.turnOn();
    fanEnabled = true;
    Serial.println("Ventilador encendido automáticamente");
    publishCurrentState();
  }
  // Si el gas baja, apagar ventilador
  else if (gasLevel < (gasThreshold - 50) && fanEnabled) {
    fan.turnOff();
    fanEnabled = false;
    Serial.println("Ventilador apagado automáticamente");
    publishCurrentState();
  }
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
  
  // Debug: Mostrar estructura del JSON recibido
  Serial.println("[DEBUG] Estructura JSON recibida:");
  serializeJsonPretty(doc, Serial);
  Serial.println();
  
  // Verificar si es respuesta de /shadow/get/accepted (estado completo)
  if (strstr(topic, "/shadow/get/accepted") != NULL) {
    Serial.println("[INFO] Recibiendo estado completo del shadow");
    
    // Verificar estructura del mensaje
    if (!doc.containsKey("state")) {
      Serial.println("[ERROR] El mensaje no contiene el campo 'state'");
      Serial.println("[DEBUG] Estructura esperada: {\"state\":{\"desired\":{...},\"reported\":{...}}}");
      return;
    }
    
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

  // Modo automático
  if (desired.containsKey("autoMode")) {
    autoMode = desired["autoMode"];
    Serial.printf("Modo automático: %s\n", autoMode ? "ON" : "OFF");
    stateChanged = true;
  }

  // Umbral de gas
  if (desired.containsKey("gasThreshold")) {
    gasThreshold = desired["gasThreshold"];
    Serial.printf("Umbral de gas ajustado a: %d ppm\n", gasThreshold);
    stateChanged = true;
  }

  // Publicar estado actualizado
  if (stateChanged) {
    publishCurrentState();
  }
}

void SmartThing::publishCurrentState() {
  StaticJsonDocument<300> doc;

  int gasLevel = gasSensor.read();

  doc["state"]["reported"]["servo"] = currentServoAngle;
  doc["state"]["reported"]["fan"] = fanEnabled;
  doc["state"]["reported"]["gasLevel"] = gasLevel;
  doc["state"]["reported"]["autoMode"] = autoMode;
  doc["state"]["reported"]["gasThreshold"] = gasThreshold;
  doc["clientToken"] = "smartThingClient";

  char buffer[512];
  serializeJson(doc, buffer);

  if (stateCallback) {
    stateCallback(buffer);
  }
}