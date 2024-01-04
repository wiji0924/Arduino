#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <WiFiUdp.h> 
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define WIFI_SSID "ngaling"  // ssid-wifi name
#define WIFI_PASSWORD "1234567890"  // wif-password

const int FrontTrigPin = 14;
const int FrontEchoPin = 12;
// defines variables
long Frontduration;

float waterLevel = 0.0;

unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

const char* serverName = "http://13.239.116.14:8090/history";

// Create AsyncWebServer object on port 80
AsyncWebServer server(8088);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<body>
    <h1>ok</h1>
</body>
</html>)rawliteral";

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  Serial.begin(9600);
    // connect to wifi.   
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  
  Serial.print("connecting");  
  while (WiFi.status() != WL_CONNECTED) {  
    Serial.print(".");  
    delay(500);  
  }  
  Serial.println();  
  Serial.print("connected: ");  
  Serial.println(WiFi.localIP());  

  pinMode(FrontTrigPin, OUTPUT); 
  pinMode(FrontEchoPin, INPUT); 

  server.on("/check", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/waterlvl", HTTP_GET, [](AsyncWebServerRequest *request) {
    String waterLVL = String(waterLevel);
    request->send(200, "application/json", "{\"water_level\": \"" + waterLVL + "\"}");
  });
  
  server.begin();
}

void loop() {

    if ((millis() - lastTime) > timerDelay) {
    GetWaterLevel();
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // JSON data to send with HTTP POST
      String httpRequestData = "{\"WaterLevel\":\"" + String(waterLevel) + "\"}";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void GetWaterLevel(){
  digitalWrite(FrontTrigPin, LOW); 
  delayMicroseconds(2);

  digitalWrite(FrontTrigPin, HIGH);
  delayMicroseconds(20);

  digitalWrite(FrontTrigPin, LOW);

  Frontduration = pulseIn(FrontEchoPin, HIGH);

  float frontdata = Frontduration * 0.034 / 2; 
  float distanceInMeters = (frontdata/100);

  float distanceMapped = floatMap(distanceInMeters, 0.2, 1.2, 0.0, 5.0);
  waterLevel = 18.5 - distanceMapped;
  Serial.println(distanceInMeters);
  delay(100);
}



