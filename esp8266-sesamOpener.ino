#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SimpleTimer.h>
#include <Bounce2.h>

#define ARRAY_LEN(a) sizeof(a)/sizeof(a[0])

#define LED_PIN D4
#define BUTTON_PIN D5
#define BUTTON_DEBOUNCE_MS 50

#define WIFI_REFRESH_STATUS_MS 700
#define WIFI_CONNECT_INTERVAL_MS 950

Bounce debouncer = Bounce();
SimpleTimer timer;

int wifiStatus = WL_DISCONNECTED;

const String SERVER_IP = "192.168.4.1";

#define WIFI_SSID "Free Candy"
#define WIFI_PASSWORD "RichardStallman"

void setup() {

  delay(1000);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  digitalWrite(LED_PIN, HIGH);
  pinMode(LED_PIN, OUTPUT);
  
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(BUTTON_DEBOUNCE_MS);

  timer.setInterval(WIFI_REFRESH_STATUS_MS, []() {
    wifiStatus = WiFi.status();    
    digitalWrite(LED_PIN, (wifiStatus == WL_CONNECTED) ? LOW : HIGH);
  });

  timer.setInterval(WIFI_CONNECT_INTERVAL_MS, []() {

    if (WiFi.SSID() != WIFI_SSID) {
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      WiFi.persistent(true);
      WiFi.setAutoConnect(true);
      WiFi.setAutoReconnect(true);
    }
  
  });
  
}

void loop() {
  timer.run();
  debouncer.update();

  if(wifiStatus == WL_CONNECTED && debouncer.fell()) {

    Serial.println("HTTP Request!");
    
    HTTPClient http;
    http.begin("http://" + SERVER_IP + "/open");
    int httpCode = http.GET();

    Serial.print("HTTP CODE ");
    Serial.println(httpCode);
    
    if(httpCode == HTTP_CODE_OK) {
      Serial.println("Everything is awesome!");
    }
  }
}
