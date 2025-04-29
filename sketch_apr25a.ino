#define BLYNK_TEMPLATE_ID "TMPL6Y-LqJAGN"
#define BLYNK_TEMPLATE_NAME "anty virus"
#define BLYNK_AUTH_TOKEN "pin-RuRcV42Rb1zgAy_DMRkGN3dwWSv7"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// WiFi Credentials
char ssid[] = "Piyanan";
char pass[] = "25110049";

// Pin Definitions
const int relayPinLight = 5;  // หลอด1
const int relayPinFan = 4;    // พัดลม
const int startButtonPin = 0; // ปุ่มเริมทำงาน
const int relayPinLight2 = 2; //หลอด2

// Timer and state management 
BlynkTimer timer;
bool systemActive = false;
unsigned long previousMillis = 0;
const unsigned long duration = 15 * 60 * 1000; // 15 minutes in milliseconds

// Function to control the whole system
void switchSystem(bool state) {
  digitalWrite(relayPinLight, state ? LOW : HIGH);
  digitalWrite(relayPinFan, state ? LOW : HIGH);
  digitalWrite(relayPinLight2, state ? LOW : HIGH);
  if (state) {
    previousMillis = millis(); // Always reset timer when turned ON
  }

  systemActive = state;
}

// การเริ่มนับเวลา
void updateCountdown() {
  if (systemActive) {
    unsigned long remaining = duration - (millis() - previousMillis);
    unsigned int minutes = remaining / 60000;
    unsigned int seconds = (remaining % 60000) / 1000;

    char timeLeft[10];
    sprintf(timeLeft, "%02d:%02d", minutes, seconds);

    Blynk.virtualWrite(V2, timeLeft); // Send to Blynk label on V2
  } else {
    Blynk.virtualWrite(V2, "00:00");
  }
}

// Blynk virtual pin control
BLYNK_WRITE(V1) {  // Virtual pin V1 for starting system from Blynk
  int pinValue = param.asInt();
  switchSystem(pinValue == 1);
}

void checkTimer() {
  if (systemActive && (millis() - previousMillis >= duration)) {
    switchSystem(false);
    Blynk.virtualWrite(V1, 0); // Sync Blynk button state
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Pins
  pinMode(relayPinLight, OUTPUT);
  pinMode(relayPinFan, OUTPUT);
  pinMode(relayPinLight2, OUTPUT);
  pinMode(startButtonPin, INPUT_PULLUP);

  digitalWrite(relayPinLight, LOW);
  digitalWrite(relayPinLight2, LOW);
  digitalWrite(relayPinFan, LOW);

  // Connect to WiFi and Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Set up timer for checking the 15-minute timeout
  timer.setInterval(1000L, checkTimer);
  timer.setInterval(1000L, updateCountdown); // Update countdown every second
}

void loop() {
  Blynk.run();
  timer.run();

  // Check physical start button
  if (digitalRead(startButtonPin) == LOW) {
    delay(50);  // Debounce
    if (digitalRead(startButtonPin) == LOW) {
      switchSystem(!systemActive);
      Blynk.virtualWrite(V1, systemActive ? 1 : 0); // Sync Blynk button state
      delay(500); // Simple debounce delay
    }
  }
}
