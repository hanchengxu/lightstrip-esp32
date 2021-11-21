#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "util.h"

#define PEOPLEPIN 34
#define LIGHTPIN 19

Adafruit_NeoPixel rgb_display = Adafruit_NeoPixel(60, 5, NEO_GRB + NEO_KHZ800);

int weatherClear[3] = {220,120,0};
int weatherNotClear[3] = {35, 120, 255};
int weatherAPINotFound[3] = {154, 156, 160};


void setLightColor(int R,int G,int B){
    for (int i = 1; i <= 60; i = i + (1)) {
      rgb_display.setPixelColor(i-1, rgb_display.Color(R,G,B));
//      rgb_display.setPixelColor(i-1, rgb_display.Color(220,120,0));
//      rgb_display.setPixelColor(i - 1, rgb_display.Color(35, 120, 255));
//      rgb_display.setPixelColor(i - 1, rgb_display.Color(154, 156, 160));
    }  
}

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
  pinMode(LIGHTPIN, INPUT);
  pinMode(PEOPLEPIN, INPUT);
}

void loop() {

  rgb_display.clear();

  int light = digitalRead(LIGHTPIN);
  int people = digitalRead(PEOPLEPIN);
  Serial.print("光线传感：");
  Serial.print(light);
  Serial.print(" ,人体传感：");
  Serial.println(people);

  //黑暗环境并且有人经过
  if (light == 1 && people ==1 ) {
    
    //天气API获取天气情报
    HTTPClient http;
    http.begin("http://www.jma.go.jp/bosai/forecast/data/forecast/130000.json");
    int httpCode = -1;
    httpCode = http.GET();
    if (httpCode > 0) {
      //得到response
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        StaticJsonDocument<512> doc;
        deserializeJson(doc, payload);
        String weatherCode = doc[0]["timeSeries"][0]["areas"][0]["weatherCodes"][0];
        Serial.print("天气code：");
        Serial.println(weatherCode);
        //天气晴朗
        if(isClearDay(weatherCode.toInt())){
          setLightColor(220,120,0);
        }else{
          //雨 雪 雷
           setLightColor(35, 120, 255);
        }
      }
    }else{
      //API 无响应
      setLightColor(154, 156, 160);
    }

    //灯带缓慢变亮
    for (int i = 1; i <= 14; i = i + (1)) {
      rgb_display.setBrightness(i * 5);
      rgb_display.show();
      delay(200);
    }
    rgb_display.show();

    //5分钟关灯
    delay(1000*60*5);

    for (int i = 1; i <= 60; i = i + (1)) {
      rgb_display.setPixelColor(i - 1, rgb_display.Color(0, 0, 0));
    }
    rgb_display.show();
  }
  
  delay(1000);

}
