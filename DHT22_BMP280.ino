#include <DHTesp.h>

#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <AsyncWebSocket.h>
#include <AsyncWebSynchronization.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>

#include <AsyncPrinter.h>
#include <DebugPrintMacros.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncTCPbuffer.h>
#include <SyncClient.h>
#include <async_config.h>
#include <tcp_axtls.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncWiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define DHTTYPE DHT21
#define TRIGGER_PIN 5

DHTesp          dht;
int             dhtPin        = 2;           
DNSServer       dns;
AsyncWebServer  server(80);


void setup(){
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  dht.setup(dhtPin, DHTesp::DHT22);
  Serial.println("Started");

  WiFi.begin();

  while ( WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect");
    delay(1000);
  }
  Serial.println(F("connected...yeey :)"));
  Serial.print(F("Local IP: "));
  Serial.println(WiFi.localIP());
 

  server.on("/temperature", HTTP_GET, [] (AsyncWebServerRequest *request) {
    TempAndHumidity   lastValues  =   dht.getTempAndHumidity();
    String            json        =   "{\"temperature\":\"%1\",\"humidity\":\"%2\"}";

    json.replace("%1",String(lastValues.temperature,2));
    json.replace("%2",String(lastValues.humidity,2));

    Serial.println(json);
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
if ( digitalRead(TRIGGER_PIN) == LOW ) {
    AsyncWiFiManager wifiManager(&server,&dns);
    WiFi.disconnect();
    if (!wifiManager.startConfigPortal("Matico-Temp-AP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      ESP.reset();
      delay(5000);
    }
    Serial.println("connected...yeey :)");
  }
}
