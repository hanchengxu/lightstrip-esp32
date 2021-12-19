#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "util.h"

#define PEOPLEPIN 34
#define LIGHTPIN 19

Adafruit_NeoPixel rgb_display = Adafruit_NeoPixel(60, 5, NEO_GRB + NEO_KHZ800);

int weatherClear[3] = {220, 120, 0};
int weatherNotClear[3] = {35, 120, 255};
int weatherAPINotFound[3] = {154, 156, 160};

//人体传感和光线传感全局变量
int light = 0;
int people = 0;
//开灯
int onLight = 0;
//正在亮灯
int lighting = 0;
//开启灯光时间
float startTime = 0.0;

//设定灯珠颜色方法
void setLightColor(int R, int G, int B) {
  for (int i = 1; i <= 60; i = i + (1)) {
    rgb_display.setPixelColor(i - 1, rgb_display.Color(R, G, B));
  }
}

//taskOne：onLight switch
void taskOne(void *parameter) {

  while (1) {
    light = digitalRead(LIGHTPIN);
    people = digitalRead(PEOPLEPIN);
    //光线暗 有人经过切 未开灯状态
    if (light == 1 && people == 1 && lighting == 0 ) {
//      Serial.println("光线传感+ 人体传感");
      //开灯
      onLight = 1;
      startTime = millis();
    }
    vTaskDelay(500);
  }
  vTaskDelete(NULL);
}

//点灯函数
void startLight() {

  HTTPClient http1;
  int httpCode = -1;
  StaticJsonDocument<512> doc;
  int hourr = 8;//默认当天上午8点
  String payload = "";

  //获取当前时间
  http1.begin("https://www.timeapi.io/api/Time/current/zone?timeZone=Asia/Tokyo");
  httpCode = http1.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      payload = http1.getString();
      deserializeJson(doc, payload);
      hourr = doc["hour"];
    }
  }

  //天气API获取天气情报
  HTTPClient http2;
  http2.begin("http://www.jma.go.jp/bosai/forecast/data/forecast/130000.json");
  int httpCode2 = http2.GET();
  if (httpCode2 > 0) {
    //得到response
    if (httpCode2 == HTTP_CODE_OK || httpCode2 == HTTP_CODE_MOVED_PERMANENTLY) {
      payload = http2.getString();
      deserializeJson(doc, payload);
      int codeIndex = 0;
      if ( hourr > 20 ) {
        //晚上8点后获取第二天天气
        codeIndex = 1;
      }
      String weatherCode = doc[0]["timeSeries"][0]["areas"][0]["weatherCodes"][codeIndex];
      //      Serial.print("天气code："); Serial.println(weatherCode);
      rgb_display.clear();
      //天气晴朗
      if (isClearDay(weatherCode.toInt())) {
        setLightColor(220, 120, 0);
      } else {
        //雨 雪 雷
        setLightColor(35, 120, 255);
      }
    } else {
      //200 以外
      setLightColor(154, 156, 160);
    }
  } else {
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

}

//亮灯函数前 预先亮起，由于请求API需要时间
void startLightPrepare() {
  setLightColor(154, 156, 160);
  rgb_display.setBrightness(10);
  rgb_display.show();
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


  pinMode(LIGHTPIN, INPUT);
  pinMode(PEOPLEPIN, INPUT);

  //子任务设定
  xTaskCreatePinnedToCore(
    taskOne,   /* Task function. */
    "taskOne", /* String with name of task. */
    15000,     /* Stack size in bytes. */
    NULL,      /* Parameter passed as input of the task */
    1,         /* Priority of the task. */
    NULL,      /* Task handle. */
    1          /* Task Core. */
  );
}

void loop() {

  //亮灯状态，检测持续亮灯时长，超过设定
  if (lighting == 1 ) {
    float tempTime = millis();
    //亮了2min
    if (tempTime - startTime > 1000 * 30) {
      rgb_display.clear();
      rgb_display.show();
      lighting = 0;
      onLight = 0;
    }
  } else {
    if (onLight == 1) {
      //亮灯状态设为1
      lighting = 1;
      onLight = 0;
      //未亮灯状态 点灯，则执行开灯函数
      startLightPrepare();
      startLight();
    }
  }

  delay(500);
}
