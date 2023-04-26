#include <FastLED.h>

#define NUM_LEDS 144

//led pin
#define DATA_PIN 5

#define PEOPLEPIN 34

CRGB leds[NUM_LEDS];

int people = 0;

int lighting = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PEOPLEPIN, INPUT);
  delay(2000);
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
}


void loop() {

   people = digitalRead(PEOPLEPIN);

   Serial.println(people);

  if(people == 1 && lighting == 0){
    lighting =1;
    for (int i= 0 ;i<70; i++){
      fill_solid(leds, 144, CHSV(85,200,i*2));
      delay(50);
      FastLED.show();
    }
  }

  if(people == 0 && lighting == 1){
    lighting =0;
    fill_solid(leds, 144, CRGB::Black);
    FastLED.show();
  }
  
  delay(500);
}
