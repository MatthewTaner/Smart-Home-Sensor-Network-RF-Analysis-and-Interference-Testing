#include <SPI.h>
#include <RF24.h>

#define CE_PIN 4
#define CSN_PIN 5

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "00001";

void setup() {
  // Start the Serial Monitor at 115200 baud (speed) for debugging
  Serial.begin(115200);
  Serial.println("--- RF24 Receiver Starting ---");

  // Initialize the radio and check for a connection
  if (!radio.begin()) {
    Serial.println("Radio hardware is not responding!!");
    while (1) {} // Halt the program if the radio isn't connected
  }
  
  // Open "mailbox" #0 to listen for mail sent to our 'address'
  radio.openReadingPipe(0, address);
  
  // Set the Power Amplifier to MAX for our PA/LNA module
  radio.setPALevel(RF24_PA_MAX);
  
  // Set the data rate to 1MBPS (must match the transmitter)
  radio.setDataRate(RF24_1MBPS);

  // Tell the radio to officially start listening
  radio.startListening();
  Serial.println("Now listening for messages...");
}

void loop() {
  // Check if there is any new mail
  if (radio.available()) {
    
    // Create a temporary "box" (buffer) to hold the message
    char text[32] = ""; 
    
    // Go get the mail and put it in our 'text' box
    radio.read(&text, sizeof(text));
    
    // Print the message to the Serial Monitor so we can see it
    Serial.print("Message Received: '");
    Serial.print(text);
    Serial.println("'");
  }
}