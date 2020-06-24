#include <WiFi.h>
#include <PubSubClient.h>

/**************************** DEBUG *******************************/

#define DEBUG_PRINTLN(m) Serial.println(m)
#define DEBUG_PRINT(m) Serial.print(m)

/**************************** Variaveis globais *******************************/

#define LED_BUILTIN 2

#define WIFI_SSID     "Zuqueto"
#define WIFI_PASSWORD "....."

#define DEVICE_TOKEN  "....."

const char* mqtt_server = "mqtt.proiot.network";

/************************* Instanciação dos objetos  **************************/

WiFiClient espClient;
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

    String clientId = "proiot-dev-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), DEVICE_TOKEN, "")) {
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

  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  Serial.print("Publish message: ");
  Serial.println("ok");

  digitalWrite(LED_BUILTIN, 1);
  client.publish("device/5ef20c1f47f0e40019a54876", "ok");
  digitalWrite(LED_BUILTIN, 0);
  delay(2000);
}
