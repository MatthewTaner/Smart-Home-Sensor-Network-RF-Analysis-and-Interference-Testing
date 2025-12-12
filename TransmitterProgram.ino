/*
 * FINAL COMBINED SENSOR TRANSMITTER
 * - nRF24 Radio
 * - PIR Motion Sensor (non-blocking)
 * - Magnetic Door Sensor (state-change)
 * - DHT11 Temp/Humidity Sensor (non-blocking)
 */

#include <SPI.h>
#include <RF24.h>
#include <DHT.h> // <-- NEW: Include the library for the DHT sensor

// --- Pin Definitions ---
// Radio
#define CE_PIN  4
#define CSN_PIN 5
// Sensors
#define PIR_PIN 13   // PIR sensor "OUT" pin
#define DOOR_PIN 12  // Door sensor pin
#define DHT_PIN 27   // <-- NEW: DHT11 data pin
#define DHT_TYPE DHT22 // <-- NEW: We are using a DHT11 sensor

// --- Radio Setup ---
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

// --- Sensor Objects ---
DHT dht(DHT_PIN, DHT_TYPE); // <-- NEW: Create a "dht" object

// --- State Variables ---
// For Door Sensor
int lastDoorState = -1; // -1 to force an update on first loop

// For Motion Sensor
int lastMotionState = LOW;
unsigned long motionTimestamp = 0;
const long motionCooldown = 5000; // 5 seconds (5000 milliseconds)

// For Temp/Humidity Sensor
unsigned long tempTimestamp = 0; // <-- NEW: Separate timestamp for the temp sensor
const long tempInterval = 10000; // <-- NEW: Check temp every 10 seconds (10000 ms)

// ------------------------------------------------------------------
//   SETUP: Runs once at the beginning
// ------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("--- Combined Sensor Node Starting ---");

  // Initialize all sensor pins
  pinMode(PIR_PIN, INPUT);
  pinMode(DOOR_PIN, INPUT_PULLUP);
  dht.begin(); // <-- NEW: Initialize the DHT sensor
  
  // Initialize Radio
  if (!radio.begin()) {
    Serial.println("Radio hardware is not responding!!");
    while (1) {} // Halt
  }
  
  // Standard transmitter setup
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.stopListening();
  
  Serial.println("Now monitoring all three sensors...");
}

// ------------------------------------------------------------------
//   LOOP: Runs thousands of times per second
// ------------------------------------------------------------------
void loop() {
  // Call a function for each sensor.
  // The loop runs fast, letting each function decide when to act.
  checkDoorSensor();
  checkMotionSensor();
  checkTempSensor(); // <-- NEW: Check the temp sensor
}

// ------------------------------------------------------------------
//   checkDoorSensor: State-change logic for the door
// ------------------------------------------------------------------
void checkDoorSensor() {
  int currentDoorState = digitalRead(DOOR_PIN);
  char message[4];

  if (currentDoorState != lastDoorState) {
    char text[32]; 

    if (currentDoorState == LOW) {
      Serial.println("DOOR: Closed. Sending update...");
      strcpy(text, "Door: Closed");
      strcpy(message,"D 0");
    } else {
      Serial.println("DOOR: Open. Sending update...");
      strcpy(text, "Door: Open");
      strcpy(message,"D 1");
    }
    
    radio.write(&message, sizeof(message));
    lastDoorState = currentDoorState;
    delay(50);
  }
}

// ------------------------------------------------------------------
//   checkMotionSensor: Non-blocking logic for the PIR
// ------------------------------------------------------------------
void checkMotionSensor() {
  int currentMotionState = digitalRead(PIR_PIN);
  char message[4] = "M 1";

  if (currentMotionState == HIGH && lastMotionState == LOW) {
    if (millis() - motionTimestamp > motionCooldown) {
      Serial.println("MOTION: Detected! Sending alert...");
      char text[] = "Motion Detected!";
      radio.write(&message, sizeof(message));
      motionTimestamp = millis();
    }
  }
  lastMotionState = currentMotionState;
}

// ------------------------------------------------------------------
//   checkTempSensor: Non-blocking logic for the DHT11
// ------------------------------------------------------------------
void checkTempSensor() {
  // Check if it's time to read the sensor (every 10 seconds)
  if (millis() - tempTimestamp > tempInterval) {
    
    // Read temperature and humidity
    float h = dht.readHumidity();
    float t = dht.readTemperature(); // Read in Celsius (default)

    // Check if any reads failed (a common issue with DHT sensors)
    if (isnan(h) || isnan(t)) {
      Serial.println("TEMP: Failed to read from DHT sensor!");
    } else {
      // Create a message string. "sprintf" is a powerful way to format text.
      // "T:23.5C, H:45.1%"
      char text[32];
      sprintf(text, "T:%.1fC, H:%.1f%%", t, h); // "%.1f" = float with 1 decimal place

      Serial.print("TEMP: ");
      Serial.print(text);
      Serial.println(". Sending update...");
      
      radio.write(&text, sizeof(text));
    }
    
    // Reset the timestamp *regardless* of success
    // This prevents it from trying to read 1000x/sec on a failure
    tempTimestamp = millis();
  }
}