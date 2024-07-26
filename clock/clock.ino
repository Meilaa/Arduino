#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD on I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins for buttons
const int modeButtonPin = 3;        // Button to switch between hour, minute, and second setting modes
const int incrementButtonPin = 2;   // Button to increment the selected setting
const int saveButtonPin = 4;        // Button to save the set time
const int resetButtonPin = 5;       // Button to reset the clock
const int alarmButtonPin = 6;       // Button to set alarm
const int buzzerPin = 8;            // Buzzer pin

enum SettingMode { HOURS, MINUTES, SECONDS, ALARM_HOURS, ALARM_MINUTES }; // Enumeration for setting modes

SettingMode currentMode = HOURS; // Current setting mode

int hours = 0;
int minutes = 0;
int seconds = 0;

int alarmHours = 0;
int alarmMinutes = 0;

bool setTime = false; // Flag to indicate if the time has been set
bool setAlarm = false; // Flag to indicate if the alarm has been set
bool displayMessage = false; // Flag to indicate if the "Time Saved" message should be displayed
unsigned long messageStartTime = 0; // Variable to store the start time of the "Time Saved" message
int timeSavedCount = 0; // Counter for the number of times the time has been saved

unsigned long previousMillis = 0;
bool showCursor = false; // Flag to indicate whether to show cursor or not

void setup() {
  // Start the I2C interface
  Wire.begin();

  // Add a delay to ensure that the LCD has enough time to initialize properly
  delay(1000);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Set button pins as inputs
  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(incrementButtonPin, INPUT_PULLUP);
  pinMode(saveButtonPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(alarmButtonPin, INPUT_PULLUP);

  // Set buzzer pin as output
  pinMode(buzzerPin, OUTPUT);

  // Display the initial time on the LCD
  displayTime();
}

void loop() {
  // Check if the mode button is pressed to switch setting modes
  if (digitalRead(modeButtonPin) == LOW) {
    delay(50); // Debouncing delay
    switch (currentMode) {
      case HOURS:
        currentMode = MINUTES;
        break;
      case MINUTES:
        currentMode = SECONDS;
        break;
      case SECONDS:
        currentMode = ALARM_HOURS;
        break;
      case ALARM_HOURS:
        currentMode = ALARM_MINUTES;
        break;
      case ALARM_MINUTES:
        currentMode = HOURS;
        break;
    }
    displayTime(); // Display the current time with the updated mode
  }

  // Check if the increment button is pressed to increment the selected setting
  if (digitalRead(incrementButtonPin) == LOW) {
    delay(50); // Debouncing delay
    switch (currentMode) {
      case HOURS:
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
        break;
      case MINUTES:
        minutes++;
        if (minutes >= 60) {
          minutes = 0;
        }
        break;
      case SECONDS:
        seconds++;
        if (seconds >= 60) {
          seconds = 0;
        }
        break;
      case ALARM_HOURS:
        alarmHours++;
        if (alarmHours >= 24) {
          alarmHours = 0;
        }
        break;
      case ALARM_MINUTES:
        alarmMinutes++;
        if (alarmMinutes >= 60) {
          alarmMinutes = 0;
        }
        break;
    }
    displayTime(); // Display the updated time
  }

  // Check if the save button is pressed to save the set time
  if (digitalRead(saveButtonPin) == LOW) {
    delay(50); // Debouncing delay
    setTime = true; // Set the flag to indicate that the time has been set
    displayMessage = true; // Set the flag to display the message
    messageStartTime = millis(); // Record the start time of the message display
    showCursor = false; // Turn off cursor for both displays
  }



  // Check if the alarm button is pressed to set the alarm
  if (digitalRead(alarmButtonPin) == LOW) {
    delay(50); // Debouncing delay
    setAlarm = !setAlarm; // Toggle the flag to indicate setting or not setting the alarm
    if (!setAlarm) { // If setting alarm is turned off, reset alarm mode to hours
      currentMode = HOURS;
    }
    displayTime(); // Display the current time with updated alarm setting mode
  }

  // Check if the reset button is pressed to reset the clock
  if (digitalRead(resetButtonPin) == LOW) {
    delay(50); // Debouncing delay
    hours = 0;
    minutes = 0;
    seconds = 0;
    alarmHours = 0; // Reset alarm hours
    alarmMinutes = 0; // Reset alarm minutes
    setTime = false; // Reset the flag to indicate that the time has not been set
    setAlarm = false; // Reset the alarm flag
    currentMode = HOURS; // Reset mode to hours
    displayTime(); // Display the reset time
  }


  // If the time has been set, start the clock
  if (setTime) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 1000) {
      previousMillis = currentMillis;
      seconds++;
      if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
          minutes = 0;
          hours++;
          if (hours >= 24) {
            hours = 0;
          }
        }
      }
      // Check if the current time matches the alarm time
      if (setAlarm && hours == alarmHours && minutes == alarmMinutes && seconds == 0) {
        lcd.clear();
        lcd.print("ALARM!");
        digitalWrite(buzzerPin, HIGH); // Turn on the buzzer
        delay(5000); // Display "ALARM!" for 10 seconds
        digitalWrite(buzzerPin, LOW); // Turn off the buzzer
        setAlarm = false; // Reset the alarm flag
        displayTime(); // Display the current time after the alarm
      }
      displayTime(); // Display the updated time
    }
  }

  // Display the "Time Saved" message if needed
  if (displayMessage) {
    unsigned long elapsedTime = millis() - messageStartTime;
    if (elapsedTime < 5000) { // Display message for 5 seconds
      lcd.setCursor(0, 0);
      lcd.print("Time Saved      ");
    } else {
      displayMessage = false; // Reset the flag
    }
  } else {
    // Display "Time:" when not showing the "Time Saved" message
    lcd.setCursor(0, 0);
    lcd.print("Time:           ");
  }

  // Show cursor if setting time or alarm
  if (!setTime || setAlarm) {
    // Only show cursor when in setting mode or setting alarm
    showCursor = true;
    lcd.blink(); // Make the cursor blink
  } else {
    showCursor = false;
    lcd.noBlink(); // Turn off cursor blinking when not setting time or alarm
  }

  // Set the cursor position based on the current setting mode
  if (showCursor) {
    switch (currentMode) {
      case HOURS:
        lcd.setCursor(0, 1); // Position the cursor at the hours on the second row
        break;
      case MINUTES:
        lcd.setCursor(3, 1); // Position the cursor at the minutes on the second row
        break;
      case SECONDS:
        lcd.setCursor(6, 1); // Position the cursor at the seconds on the second row
        break;
      case ALARM_HOURS:
        lcd.setCursor(9, 1); // Position the cursor at the alarm hours on the second row
        break;
      case ALARM_MINUTES:
        lcd.setCursor(12, 1); // Position the cursor at the alarm minutes on the second row
        break;
    }
    lcd.blink(); // Make the cursor blink
  }

  delay(100); // Add a small delay to debounce buttons
}

void displayTime() {
  // Clear the previous cursor for both displays
  lcd.noBlink();

  // Display time on the primary display
  lcd.setCursor(0, 1);
  if (hours < 10) {
    lcd.print("0");
  }
  lcd.print(hours);
  lcd.print(':');
  if (minutes < 10) {
    lcd.print("0");
  }
  lcd.print(minutes);
  lcd.print(':');
  if (seconds < 10) {
    lcd.print("0");
  }
  lcd.print(seconds);

  // Display alarm time on the secondary display if setting alarm
  if (setAlarm) {
    lcd.setCursor(9, 1);
    if (alarmHours < 10) {
      lcd.print("0");
    }
    lcd.print(alarmHours);
    lcd.print(':');
    if (alarmMinutes < 10) {
      lcd.print("0");
    }
    lcd.print(alarmMinutes);
  }

  // Re-position the cursor for both displays after updating the time
  if (showCursor) {
    switch (currentMode) {
      case HOURS:
        lcd.setCursor(0, 1); // Position the cursor at the hours on the primary display
        break;
      case MINUTES:
        lcd.setCursor(3, 1); // Position the cursor at the minutes on the primary display
        break;
      case SECONDS:
        lcd.setCursor(6, 1); // Position the cursor at the seconds on the primary display
        break;
      case ALARM_HOURS:
        lcd.setCursor(9, 1); // Position the cursor at the alarm hours on the secondary display
        break;
      case ALARM_MINUTES:
        lcd.setCursor(12, 1); // Position the cursor at the alarm minutes on the secondary display
        break;
    }
    lcd.blink(); // Make the cursor blink for both displays
  }
}
