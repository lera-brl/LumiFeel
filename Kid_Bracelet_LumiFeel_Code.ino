#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

// Wi-Fi Credentials
const char* ssid = "9899 Hyperoptic Fibre Broadband";   //  WIFI Name
const char* password = "g6twJkdwMqvk";    //WIFI Password

// Settings for the UDP
WiFiUDP udp;
const int udpPort = 3333;   //Port
const char* parentIP = "192.168.1.120";    //Parent Bracelet's IP address

// Pin Setup
const int buttonPin = 18; 
const int ledPin = 27;    
Adafruit_NeoPixel strip(1, ledPin, NEO_GRB + NEO_KHZ800);  // First Led
bool lastButtonState = HIGH;   // Last
bool ledState = LOW;         // Current

void setup() {
  Serial.begin(115200); // Serial Monitor 115200!!!
  delay(1000); 

  
  Serial.println("Connecting to Wi-Fi..;)");        //WIFI
  WiFi.begin(ssid, password);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Wi-Fi Status: ");
    Serial.println(WiFi.status());  // Wi-Fi status 
  }

  // Connection is happening
  Serial.println("Kid Bracelet connected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 

  udp.begin(udpPort);   // Start UDP 

  pinMode(buttonPin, INPUT_PULLUP); 
  strip.begin(); 
  strip.show();   
}

void loop() {
  // Wi-Fi status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi Status: ");
    Serial.println(WiFi.status()); 
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());  // Kid IP Address 
  } else {
    Serial.println("Wi-Fi not connected.");
  }

  // Button presses
  bool buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastButtonState == HIGH) { 
    ledState = !ledState; // Toggle LED state
    updateLED(); // Update LED locally
    sendUDPMessage(ledState ? "ON" : "OFF");   //Kid  -  Parent connection
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

  delay(1000); 
}

// LED
void updateLED() {
  if (ledState) {
    strip.setPixelColor(0, strip.Color(255, 165, 0));  // Orange
    strip.show();
    Serial.println("LED turned ON");
  } else {
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
    Serial.println("LED turned OFF");
  }
}


void sendUDPMessage(const char* message) {
  udp.beginPacket(parentIP, udpPort);
  udp.print(message);
  udp.endPacket();
}
