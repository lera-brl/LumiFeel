#include <Adafruit_NeoPixel.h>

#define LED_RING_PIN 27  // Pin connected to LED Ring Data IN
#define BUTTON_PIN 18    // Pin connected to Parent Button
#define NUM_LEDS 12      // Number of LEDs in the ring

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUM_LEDS, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button input with internal pull-up
  ring.begin();
  ring.show(); // Initialize all pixels to 'off'
  ring.fill(ring.Color(0, 255, 0)); // Set LED ring to green
  ring.show();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    // If button is pressed, change LED ring to red
    ring.fill(ring.Color(255, 0, 0)); // Set to red
    ring.show();
  } else {
    // If button is not pressed, keep it green
    ring.fill(ring.Color(0, 255, 0)); // Set to green
    ring.show();
  }
}