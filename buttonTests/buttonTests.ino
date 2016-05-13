/*
 * Testing Interrupt with different flags
 * based on place in program
 * 
 * Required Libraries:  FastLED
 */

#include "FastLED.h"

#define BUTTON_PIN 3
#define DATA_PIN 2
#define NUM_LEDS 8
#define BRIGHTNESS 48

int stateMachine = 0;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;

unsigned long flashTime = millis();
int cal = 1;

volatile bool pattern = false;
volatile bool select = false;
volatile bool normalPress = false;
int whichPattern = 0;

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812,DATA_PIN,GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  turnOnLeds();

  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr, RISING);

  Serial.begin(9600);

}

void loop() {
  
  if (normalPress) {
    FastLED.delay(1000);
    switch(stateMachine) {
      case 0:
        turnOnRpm(6);
        FastLED.delay(1000);
        flashLeds(CRGB::Green);
        stateMachine++;
        break;
      case 1:
        flashLeds(CRGB::Blue);
        stateMachine++;
        break;
      case 2:
        pattern = true;
        select = false;

        if (!select) {
          turnOnRpm(1);
          whichPattern = 1;
          FastLED.delay(3000);
        }
        if (!select) {
          turnOnRpm(2);
          whichPattern = 2;
          FastLED.delay(3000);
        }
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        FastLED.delay(200);
        pattern = false;
        stateMachine++;
        break;
      case 3:
        switch(whichPattern) {
          case 0:
            turnOnRpm(7);
            break;
          case 1:
            turnOnRpm(1);
            break;
          case 2:
            turnOnRpm(2);
            break;
        }
        FastLED.delay(1000);
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        FastLED.delay(200);
        stateMachine = 0;
        break;
      
    }
    normalPress = false;
  }

}


boolean buttonDebounce(){
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState){
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        lastButtonState = reading;
        return true;
      }
    }
  }
  lastButtonState = reading;
  return false;
}

/*
 * turnOnLeds: Statup routine for the LEDs
 *              Fade green to red up and off down
 */
void turnOnLeds() {
  int hue = 96;
  for (int dot = 0; dot < NUM_LEDS; dot++){
    leds[dot] = CHSV(hue, 255, 255);
    delay(100);
    FastLED.show();

    hue -= 96/(NUM_LEDS-1);
  }

  delay(500);

  for (int dot = NUM_LEDS - 1; dot > -1; dot--){
    leds[dot] = CRGB::Black;
    FastLED.show();
    delay(100);
  }
}

void turnOnRpm(int howMany) {
  for(int dot = 0; dot < howMany; dot++) {
    dot < 4 ? leds[dot] = CRGB::Green : leds[dot] = CRGB::Yellow;
    if(dot > 5) leds[dot] = CRGB::Red;
  }
  FastLED.show();
}

void flashLeds(CRGB color) {
  for(int x = 0; x < 2; x++){
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
    FastLED.delay(200);
  
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    FastLED.delay(200);
  }
}

void isr() {
  if (pattern) {
    select = true;
  } else {
    normalPress = true;
  }
}
