#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Reemplaza con tus credenciales de red WiFi
const char* ssid = "ssid";
const char* password = "password";

// Dirección IP del broker MQTT
const char* mqtt_server = "ip";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

#define SENSOR_PIN A0  // Pin donde se conecta el sensor FC-28

unsigned long previousMillis = 0;  
const long interval = 10000;       // Intervalo para actualizaciones del sensor (10 segundos)

void setup_wifi() {
  delay(10);
  //Serial.print("Conectando a ");
  //Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  /*Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());*/
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (mqttClient.connect("ESP8266Client", "admin", "lokodeldiablo")) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intenta de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Leer el valor del sensor FC-28
    int sensorValue = analogRead(SENSOR_PIN);
    float moisturePercentage = map(sensorValue, 1023, 0, 0, 100); // Ajustar según calibración

    String message = String(moisturePercentage);
    String sensorVal = String(sensorValue);
    // Publicar el valor de humedad
    mqttClient.publish("test", message.c_str());

    /* Descomenta para depuración 
    Serial.print("Humedad del suelo cruda: ");
    Serial.print(sensorVal);
    Serial.print(", Humedad del suelo: ");
    Serial.print(moisturePercentage);
    Serial.println("%");*/
    
  }
}
