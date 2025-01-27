#include <Adafruit_NeoPixel.h>

//Kid LED 
#define LED_STICK_PIN 6        
#define NUM_STICK_LEDS 8       

//Parent LED 
#define LED_RING_PIN 7         
#define NUM_RING_LEDS 12       

// Button setup
#define KIDS_BUTTON_PIN 2         // Kids Button
#define PARENT_BUTTON_PIN 3      // Parent Button

Adafruit_NeoPixel stick = Adafruit_NeoPixel(NUM_STICK_LEDS, LED_STICK_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUM_RING_LEDS, LED_RING_PIN, NEO_GRB + NEO_KHZ800);


bool emergencyMode = false;            
bool parentLightShowActive = false;    
unsigned long parentLightShowStart = 0; 
unsigned long emergencyStartTime = 0;  
bool checkingForEmergency = false;     
int kidsButtonState = 0;               
int previousKidsState = 0;             

void setup() {
  stick.begin();               
  ring.begin();                
  stick.show();                
  ring.show();                 
  pinMode(KIDS_BUTTON_PIN, INPUT_PULLUP);  
  pinMode(PARENT_BUTTON_PIN, INPUT_PULLUP); 

  Serial.begin(9600);          //9600 - SM
  Serial.println("Setup complete.");
}

void loop() {
  unsigned long currentTime = millis();

  
  handleEmergencyMode(currentTime);

  
  if (emergencyMode) {
    ambulanceBlinkingRedAlertEffect(); 
    return; 
  }

  
  handleKidsButton();

  
  handleParentButton(currentTime);

  
  if (parentLightShowActive) {
    if (currentTime - parentLightShowStart < 5000) {
      runParentLightShow(); // 5 seconds
    } else {
      parentLightShowActive = false; 
      restoreSpecificPreviousState(); 
    }
    return; // Freeze 
  }
}


void handleEmergencyMode(unsigned long currentTime) {
  
  if (digitalRead(KIDS_BUTTON_PIN) == LOW || digitalRead(PARENT_BUTTON_PIN) == LOW) {
    if (!checkingForEmergency) {
      checkingForEmergency = true;
      emergencyStartTime = currentTime; // Start timing
    } else if (currentTime - emergencyStartTime >= 5000) {
      
      emergencyMode = !emergencyMode;
      checkingForEmergency = false; 
      Serial.println(emergencyMode ? "Emergency Mode ON" : "Emergency Mode OFF");

      
      stick.clear();
      ring.clear();
      stick.show();
      ring.show();
    }
  } else {
    checkingForEmergency = false; 
  }
}


void ambulanceBlinkingRedAlertEffect() {
  stick.clear();
  ring.clear();

  for (int i = 0; i < NUM_STICK_LEDS; i++) {
    int randomBrightness = random(30, 51); 
    stick.setPixelColor(i, stick.Color(randomBrightness, 0, 0));
  }

  for (int i = 0; i < NUM_RING_LEDS; i++) {
    int randomBrightness = random(30, 51); 
    ring.setPixelColor(i, ring.Color(randomBrightness, 0, 0));
  }

  stick.show();
  ring.show();
  delay(200);

  stick.clear();
  ring.clear();
  stick.show();
  ring.show();
  delay(100);
}


void handleKidsButton() {
  if (digitalRead(KIDS_BUTTON_PIN) == LOW && !parentLightShowActive) { 
    delay(200); 
    kidsButtonState = (kidsButtonState + 1) % 4;

    switch (kidsButtonState) {
      case 0: 
        sequentialLightsOff(NUM_STICK_LEDS, NUM_RING_LEDS);
        break;
      case 1: 
        smoothTransition(3, stick.Color(0, 51, 0), 4, ring.Color(0, 51, 0));
        break;
      case 2: 
        smoothTransition(5, stick.Color(0, 0, 51), 8, ring.Color(0, 0, 51));
        break;
      case 3: 
        smoothTransition(8, stick.Color(51, 0, 0), 12, ring.Color(51, 0, 0));
        break;
    }

    stick.show();
    ring.show();
  }
}


void handleParentButton(unsigned long currentTime) {
  if (digitalRead(PARENT_BUTTON_PIN) == LOW && !parentLightShowActive && !emergencyMode) {
    delay(200); 
    parentLightShowActive = true;
    parentLightShowStart = currentTime;

    
    previousKidsState = kidsButtonState;
  }
}


void runParentLightShow() {
  uint32_t colors[] = {
    stick.Color(51, 0, 0),    
    stick.Color(0, 51, 0),    
    stick.Color(0, 0, 51),    
    stick.Color(51, 51, 0),   
    stick.Color(25, 0, 25)    
  };

  for (int cycle = 0; cycle < NUM_STICK_LEDS; cycle++) {
    for (int i = 0; i < NUM_STICK_LEDS; i++) {
      stick.setPixelColor(i, colors[(i + cycle) % 5]);
    }
    stick.show();

    for (int i = 0; i < NUM_RING_LEDS; i++) {
      ring.setPixelColor(i, colors[(i + cycle) % 5]);
    }
    ring.show();

    delay(100); 
  }
}


void smoothTransition(int numStick, uint32_t stickColor, int numRing, uint32_t ringColor) {
  for (int i = 0; i < max(NUM_STICK_LEDS, NUM_RING_LEDS); i++) {
    if (i < numStick) {
      stick.setPixelColor(i, stickColor);
    } else {
      stick.setPixelColor(i, stick.Color(0, 0, 0));
    }

    if (i < numRing) {
      ring.setPixelColor(i, ringColor);
    } else {
      ring.setPixelColor(i, ring.Color(0, 0, 0));
    }

    stick.show();
    ring.show();
    delay(150); 
  }
}


void sequentialLightsOff(int numStick, int numRing) {
  for (int i = 0; i < max(numStick, numRing); i++) {
    if (i < numStick) {
      stick.setPixelColor(i, stick.Color(0, 0, 0));
    }
    if (i < numRing) {
      ring.setPixelColor(i, ring.Color(0, 0, 0));
    }

    stick.show();
    ring.show();
    delay(100);
  }
}


void restoreSpecificPreviousState() {
  switch (previousKidsState) {
    case 0:
      sequentialLightsOff(NUM_STICK_LEDS, NUM_RING_LEDS);
      break;
    case 1:
      smoothTransition(3, stick.Color(0, 51, 0), 4, ring.Color(0, 51, 0));
      break;
    case 2:
      smoothTransition(5, stick.Color(0, 0, 51), 8, ring.Color(0, 0, 51));
      break;
    case 3:
      smoothTransition(8, stick.Color(51, 0, 0), 12, ring.Color(51, 0, 0));
      break;
  }
}
