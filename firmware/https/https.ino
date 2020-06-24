/************************** Inclusão das Bibliotecas **************************/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/**************************** DEBUG *******************************/

#define DEBUG_PRINTLN(m) Serial.println(m)
#define DEBUG_PRINT(m) Serial.print(m)

/**************************** Variaveis globais *******************************/

// https://things.proiot.network/stream/device/dev-01/variable/01/25.5

#define LED_BUILTIN 2

#define WIFI_SSID     "Zuqueto"
#define WIFI_PASSWORD "....."

#define DEVICE_NAME     "dev-01"
#define DEVICE_TOKEN    "....."
#define VARIABLE_ALIAS  "01"

String API_URL = "http://things.proiot.network/stream";

/************************* Declaração dos Prototypes **************************/

void initSerial();
void initWiFi();
bool sendData();
void handleError(int httpCode , String message);

/************************* Instanciação dos objetos  **************************/

WiFiClientSecure client;
HTTPClient http;

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

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

String getTemperature() {
  float temperature = ((temprature_sens_read() - 32) / 1.8);

  return String(temperature);
}

bool sendData() {
  String uri = API_URL + "/device/" + DEVICE_NAME + "/variable/" + VARIABLE_ALIAS + "/" + getTemperature();

  http.begin(uri);

  http.addHeader("Authorization", DEVICE_TOKEN);

  int httpCode = http.POST("");

  String response =  http.getString();
  http.end();


  if (httpCode != HTTP_CODE_OK) {
    handleError(httpCode, response);
    return false;
  }

  DEBUG_PRINTLN("[SENSOR] " + response);

  return true;
}

/********************************** Sketch ************************************/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  initSerial();
  initWiFi();
}

void loop() {
  digitalWrite(LED_BUILTIN, 1);
  sendData();
  digitalWrite(LED_BUILTIN, 0);

  delay(5000);
}

