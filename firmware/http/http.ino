/************************** Inclusão das Bibliotecas **************************/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/**************************** DEBUG *******************************/

#define DEBUG_PRINTLN(m) Serial.println(m)
#define DEBUG_PRINT(m) Serial.print(m)

/**************************** Variaveis globais *******************************/

#define LED_BUILTIN 2

#define WIFI_SSID     "Zuqueto"
#define WIFI_PASSWORD "PASSWORD"

#define DEVICE_EUI    "DEVICE_EUI"
#define DEVICE_TOKEN   "TOKEN"

String API_URL = "http://things.conn.proiot.network";

unsigned long previousMillis = 0;

int interval = 5000; // 60 segundos

/************************* Declaração dos Prototypes **************************/

void initSerial();
void initWiFi();
void sendData();
void handleError(int httpCode , String message);

/************************* Instanciação dos objetos  **************************/

WiFiClient client;
HTTPClient http;

void initSerial() {
  Serial.begin(115200);
}

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

void handleError(int httpCode , String message ) {
  DEBUG_PRINT("Code: " + String(httpCode));
  DEBUG_PRINT(" | code msg: " + http.errorToString(httpCode));
  DEBUG_PRINTLN(" | Message: " + message);
}



/********************************** Sketch ************************************/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  initSerial();
  initWiFi();
}

void loop() {
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
  String uri = API_URL + "/" + DEVICE_EUI;

  http.begin(uri);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", DEVICE_TOKEN);

  String payload = getPayload();

  int httpCode = http.POST(payload);

  if (httpCode != HTTP_CODE_NO_CONTENT) {
    String response =  http.getString();

    handleError(httpCode, response);

    http.end();

    return;
  }

  DEBUG_PRINTLN("[SENSOR] ok");
}

