#include <DHT.h>
#include <EEPROM.h>

#define DHTPIN 7             // Pin where the DHT22 sensor is connected
#define DHTTYPE DHT22        // DHT22 sensor type

#define EEPROM_SIZE 1024     // Size of EEPROM
#define DATA_SIZE 12         // Size of data (humidity + temperature + timestamp)
#define INTERVAL 300         // Time interval for saving data to EEPROM (300 milliseconds)
#define BUTTON_PIN 2         // Pin connected to the button
#define CLEAR_BUTTON_PIN 3   // Pin connected to the button for clearing EEPROM data

DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillis = 0;
int address = 0;             // Current address in the EEPROM
bool buttonState = false;    // Current state of the button
bool lastButtonState = false; // Previous state of the button
bool eepromFull = false;     // Flag to indicate if EEPROM is full
unsigned long debounceDelay = 50; // Debounce delay in milliseconds
unsigned long lastDebounceTime = 0; // Time of the last button state change
unsigned long lastClearDebounceTime = 0; // Time of the last clear button state change
bool lastClearButtonState = false; // Previous state of the clear button

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(CLEAR_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;
    
    float humidity = dht.readHumidity();         // Read humidity (in %)
    float temperature = dht.readTemperature();   // Read temperature (in Celsius)

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Save the data to EEPROM along with the timestamp
    unsigned long timestamp = millis() / 1000; // Convert milliseconds to seconds
    writeDataToEEPROM(humidity, temperature, timestamp);
  }

  // Check if EEPROM is full
  if (!eepromFull && address + DATA_SIZE > EEPROM_SIZE) {
    eepromFull = true; // Set the flag indicating EEPROM is full
    address = 0;       // Reset address to start from the beginning
    Serial.println("EEPROM is full.");
    readAllDataFromEEPROM();
  }

  // Read the state of the button
  int reading = digitalRead(BUTTON_PIN);

  // Check if the button state has changed
  if (reading != lastButtonState) {
    // Reset the debounce timer
    lastDebounceTime = currentMillis;
  }

  // Check if the debounce delay has passed
  if (currentMillis - lastDebounceTime >= debounceDelay) {
    // Update the button state only if it has been stable
    if (reading != buttonState) {
      buttonState = reading;

      // Check if the button is pressed
      if (buttonState == LOW) {
        Serial.println("Button is pressed, reading EEPROM data...");
        readAllDataFromEEPROM();
      }
    }
  }

  // Save the current button state for the next iteration
  lastButtonState = reading;

  // Read the state of the clear button
  int clearButtonState = digitalRead(CLEAR_BUTTON_PIN);

      // Check if the clear button is pressed
      if (clearButtonState == LOW) {
        Serial.println("Clear button is pressed...");
        clearEEPROM();
        delay(1000); // Delay to prevent multiple executions due to bouncing
      }
}

void writeDataToEEPROM(float humidity, float temperature, unsigned long timestamp) {
  // // Check if we've reached the end of EEPROM
  // if (!eepromFull && address + DATA_SIZE > EEPROM_SIZE) {
  //   eepromFull = true; // Set the flag indicating EEPROM is full
  //   address = 0;       // Reset address to start from the beginning
  //   Serial.println("EEPROM is full.");
  //   readAllDataFromEEPROM();
  // }
  
  EEPROM.put(address, humidity);
  EEPROM.put(address + sizeof(float), temperature);
  EEPROM.put(address + 2 * sizeof(float), timestamp);
  address += DATA_SIZE; // Move to the next address space for future data
}

void readAllDataFromEEPROM() {
  Serial.println("EEPROM Data:");

  // Iterate through each entry and print data
  for (int i = 0; i < EEPROM_SIZE; i += DATA_SIZE) {
    float humidity, temperature;
    unsigned long timestamp;
    EEPROM.get(i, humidity);
    EEPROM.get(i + sizeof(float), temperature);
    EEPROM.get(i + 2 * sizeof(float), timestamp);

    // Send data over serial in the format: timestamp,humidity,temperature
    Serial.print(timestamp);
    Serial.print(",");
    Serial.print(humidity);
    Serial.print(",");
    Serial.println(temperature);
  }
}

void clearEEPROM() {
  // Reset address to start from the beginning
  address = 0;

  // Clear each cell of EEPROM by writing 0xFF
  for (int i = 0; i < EEPROM_SIZE; ++i) {
    EEPROM.write(i, 0xFF);
  }

  // Reset the flag indicating EEPROM is not full
  eepromFull = false;
}
