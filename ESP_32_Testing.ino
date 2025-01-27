#include <Adafruit_NeoPixel.h>

#define LED_RING_PIN 27  
#define BUTTON_PIN 18    
#define NUM_LEDS 12      

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUM_LEDS, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  ring.begin();
  ring.show(); 
  ring.fill(ring.Color(0, 255, 0)); //Green Colour
  ring.show();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {   //Green Colour to Red Colour
    
    ring.fill(ring.Color(255, 0, 0)); // Red Colour
    ring.show();
  } else {
    
    ring.fill(ring.Color(0, 255, 0));     // Green
    ring.show();
  }
}
