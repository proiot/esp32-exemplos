/************************** Inclusão das Bibliotecas **************************/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/**************************** DEBUG *******************************/

#define DEBUG_PRINTLN(m) Serial.println(m)
#define DEBUG_PRINT(m) Serial.print(m)

/**************************** Variaveis globais *******************************/

#define LED_BUILTIN 2

#define WIFI_SSID     "Zuqueto"
#define WIFI_PASSWORD "PASSWORD"

String device_token   = "TOKEN";
String device_hashapp = "DEVICE_HASH_APP";
String device_eui     = "DEVICE_EUI";

const char* mqtt_server = "mqtt.proiot.network";
const int mqtt_port     = 8883;

unsigned long previousMillis = 0;

int interval = 5000; // 60 segundos

/************************* Instanciação dos objetos  **************************/

WiFiClientSecure espClient;
PubSubClient client(espClient);

/********************************** Sketch ************************************/

void initWiFi() {

  delay(10);
  DEBUG_PRINTLN("");
  DEBUG_PRINT("[WIFI] Conectando-se em " + String(WIFI_SSID));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_PRINT(".");
  }

  DEBUG_PRINTLN("");
  DEBUG_PRINT(F("[WIFI] SSID: "));
  DEBUG_PRINTLN(WIFI_SSID);
  DEBUG_PRINT(F("[WIFI] IP: "));
  DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINT(F("[WIFI] Mac: "));
  DEBUG_PRINTLN(WiFi.macAddress());
  DEBUG_PRINTLN("");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(device_eui.c_str(), device_token.c_str(), "")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  Serial.begin(115200);

  initWiFi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    digitalWrite(LED_BUILTIN, 1);
    sendData();
    digitalWrite(LED_BUILTIN, 0);

    previousMillis = currentMillis;
  }
}

String getTemperature() {
  float value = random(20.0, 35.5);

  return String(value);
}

String getBattery() {
  float value = random(0, 100);

  return String(value);
}

String getPayload() {
  DynamicJsonDocument doc(128);

  JsonArray data = doc.createNestedArray("data");

  JsonObject temmperature = data.createNestedObject();
  JsonObject battery = data.createNestedObject();

  temmperature["name"] = "temp";
  temmperature["value"] = getTemperature();

  battery["name"] = "bat";
  battery["value"] = getBattery();

  String payload;

  serializeJson(doc, payload);

  return payload;
}

void sendData() {
  String topic = "proiot/" + device_hashapp + "/" + device_eui + "/tx";
  String payload = getPayload();

  client.publish(topic.c_str(), payload.c_str());

  DEBUG_PRINTLN("[SENSOR] ok");
}

