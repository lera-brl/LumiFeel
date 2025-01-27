#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

// Wi-Fi Credentials
const char* ssid = "...";   //WIFI Name
const char* password = "...";    //WIFi  Password

// UDP Settings
WiFiUDP udp;
const int udpPort = 3333; 
const char* kidIP = "192.168.1.119"; // IP address - Kid

// Pin Setup
const int buttonPin = 18; 
const int ledPin = 27;    
Adafruit_NeoPixel strip(1, ledPin, NEO_GRB + NEO_KHZ800); 
bool lastButtonState = HIGH; 
bool ledState = LOW;         

void setup() {
  Serial.begin(115200); 
  delay(1000); 

  //Wi-Fi
  Serial.println("Connecting to Wi-Fi..;)");
  WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Wi-Fi Status: ");
    Serial.println(WiFi.status()); 
  }

  // Connection successful
  Serial.println("Parent Bracelet connected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 

  udp.begin(udpPort); 

  pinMode(buttonPin, INPUT_PULLUP); 
  strip.begin(); 
  strip.show();  
}

void loop() {
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi Status: ");
    Serial.println(WiFi.status()); 
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); 
  } else {
    Serial.println("Wi-Fi not connected.");
  }

  // Monitor button presses
  bool buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastButtonState == HIGH) { 
    ledState = !ledState; // Toggle LED state
    updateLED(); // Update LED locally
    sendUDPMessage(ledState ? "ON" : "OFF"); 
  }

  lastButtonState = buttonState; 

  
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = '\0';
    Serial.printf("Received: %s\n", incomingPacket);

    
    if (strcmp(incomingPacket, "ON") == 0) {
      ledState = HIGH;
      updateLED();
    } else if (strcmp(incomingPacket, "OFF") == 0) {
      ledState = LOW;
      updateLED();
    }
  }

  delay(1300); 
}


void updateLED() {
  if (ledState) {
    strip.setPixelColor(0, strip.Color(255, 165, 0)); // Orange
    strip.show();
    Serial.println("LED turned ON");
  } else {
    strip.setPixelColor(0, strip.Color(0, 0, 0)); 
    strip.show();
    Serial.println("LED turned OFF");
  }
}


void sendUDPMessage(const char* message) {
  udp.beginPacket(kidIP, udpPort);
  udp.print(message);
  udp.endPacket();
}

