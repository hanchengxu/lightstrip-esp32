#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

Adafruit_NeoPixel rgb_display = Adafruit_NeoPixel(60, 18, NEO_GRB + NEO_KHZ800);

void setup() {

  Serial.begin(115200);

  //wifi setting
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //lightstrip setting
  rgb_display.begin();
  rgb_display.show();
  rgb_display.setBrightness(0);
  
  //
  pinMode(19, INPUT);
  pinMode(21, INPUT);
}

void loop() {

  rgb_display.clear();
  
  // put your main code here, to run repeatedly:
  //  delay(1000*30);
  HTTPClient http;
  http.begin("http://www.jma.go.jp/bosai/forecast/data/forecast/130000.json");
  int httpCode = -1;
  //  httpCode = http.GET();
  //  Serial.print("response coede:");
  //  Serial.println(httpCode);
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = http.getString();
      StaticJsonDocument<512> doc;
      deserializeJson(doc, payload);
      String weatherCodes = doc[0]["timeSeries"][0]["areas"][0]["weatherCodes"][0];
      Serial.println(weatherCodes);
    }
  }

  int light = digitalRead(19);
  int people = digitalRead(21);
  Serial.print("光线传感：");
  Serial.println(light);
  Serial.print("人体传感：");
  Serial.println(people);

  if (light == 1) {
    for (int i = 1; i <= 60; i = i + (1)) {
      //        rgb_display.setPixelColor(i-1, rgb_display.Color(220,120,0));
//      rgb_display.setPixelColor(i - 1, rgb_display.Color(35, 120, 255));
      rgb_display.setPixelColor(i - 1, rgb_display.Color(154, 156, 160));
    }
    for (int i = 1; i <= 14; i = i + (1)) {
      rgb_display.setBrightness(i * 5);
      rgb_display.show();
      delay(100);
    }
    rgb_display.show();

    //5分钟关灯
    //  delay(1000*60*5);
    delay(2000);


    for (int i = 1; i <= 60; i = i + (1)) {
      rgb_display.setPixelColor(i - 1, rgb_display.Color(0, 0, 0));
    }
    rgb_display.show();

  }
  delay(1000);

}
