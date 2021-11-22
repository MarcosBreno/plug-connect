#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 


#ifdef ESP8266 
      
#endif 
#ifdef ESP32   
       
#endif

#define FIREBASE_HOST "plugconnect-7e839-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "tMoumLvxNEy2GlCrR1Zcr6WwmIsa9RZKd8sD5ZKi"
#define WIFI_SSID "Cavalcante"
#define WIFI_PASSWORD "15052406"
#define APP_KEY    "70f545bd-5478-4fca-a36e-a8cc30a774da"
#define APP_SECRET "e7975446-2a1d-4a35-a89b-08e51f5b95db-1f916882-20c0-4720-9851-cb176f111364"

struct RelayInfo {
  String deviceId;
  String name;
  int pin;
}

std::vector<RelayInfo> relays = {
    {"619ac8a90e8d611820f55351", "Luz 1", 0},
    {"619ac9a6eb3dca1828238feb", "Luz 2", 1},}

bool onPowerState(const String &deviceId, bool &state) {
  for (auto &relay : relays) {                                                            
    if (deviceId == relay.deviceId) {                                                       
      Serial.printf("Dispositivo %s está %s\r\n", relay.name.c_str(), state ? "on" : "off");     
      digitalWrite(relay.pin, state);                                                         
      return true;
    }                                                                              
  }
  return false; 
}

void setupRelayPins() {
  for (auto &relay : relays) {    
    pinMode(relay.pin, OUTPUT);     
  }
}

void setupSinricPro() {
  for (auto &relay : relays) {                            
    SinricProSwitch &mySwitch = SinricPro[relay.deviceId];   
    mySwitch.onPowerState(onPowerState);                     
  }
  SinricPro.onConnected([]() { Serial.printf("Conectado ao SinricPro\r\n"); });
  SinricPro.onDisconnected([]() { Serial.printf("Desconectado do SinricPro\r\n"); });

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
Serial.begin(9600);
setupRelayPins();
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("Conectando");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(500);
}
setupSinricPro();
Serial.println();
Serial.print("Conectado: ");
Serial.println(WiFi.localIP());
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
pinMode(0, OUTPUT);
}
int n = 0;
void loop() {
 
String val = Firebase.getString("relay");
String auth = Firebase.getString("auth");
Serial.print("Relay: ");
Serial.print("Auth: ");
Serial.println(val);
Serial.println(auth);
if (auth == "1" || "0"){
if (val == "0"){
  digitalWrite(0, LOW);
} else if (val == "1"){
  digitalWrite(0, HIGH);
} else {
  Serial.print("Problema no Banco de Dados\n");
  Serial.print(val);
  digitalWrite(0, LOW);
  }
}
SinricPro.handle();
delay(2000);
}
