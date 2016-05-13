/*
 * Testing blinking functions
 */

#include "FastLED.h"

#define DATA_PIN 2
#define NUM_LEDS 8
#define BRIGHTNESS 48

unsigned long flashTime = millis();

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812,DATA_PIN,GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

}

void loop() {
  
  if (millis() - flashTime < 200) {
    for (int x = 0; x < 4; x++){
      x < 3 ? leds[x] = CRGB::Green : leds[x] = CRGB::Yellow;
      leds[x+4] = CRGB::Black;
    }
  } else if (millis() - flashTime < 400) {
    for(int x = 0; x < 4; x++) {
      x < 2 ? leds[x+4] = CHSV(64, 255, 255) : leds[x+4] = CHSV(0, 255, 255);
      leds[x] = CRGB::Black;
    }
  } else {
    flashTime = millis();
  }
  FastLED.show();

}
