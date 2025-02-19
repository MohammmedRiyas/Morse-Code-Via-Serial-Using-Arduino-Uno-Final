#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x3F for a 20x4 LCD (this could vary for your display)
// If it doesn't work, try changing 0x3F to 0x27, 0x20, or other addresses.
LiquidCrystal_I2C lcd(0x3F, 20, 4);  // Initialize the LCD (20 columns, 4 rows)

// Pin assignments for various functions
int tonePin = 9;  // Arduino pin for tone (use a PWM-capable pin)
int ledPin = 13;  // Arduino pin for LED (used for visual feedback)
int buttonPin1 = 2;  // Arduino pin for Button 1 (input button for Morse code)
int buttonPin2 = 3;  // Arduino pin for Button 2 (input button for Morse code)
int transmitPin1 = 4;  // Arduino pin for Transmit Button 1 (input pin for transmit control)
int transmitPin2 = 5;  // Arduino pin for Transmit Button 2 (input pin for transmit control)
int transmitLed1 = 6;  // Arduino pin for Transmit LED 1 (visual indicator of Transmit 1)
int transmitLed2 = 7;  // Arduino pin for Transmit LED 2 (visual indicator of Transmit 2)

bool transmitActive1 = false;  // Flag to track whether Transmit 1 is active
bool transmitActive2 = false;  // Flag to track whether Transmit 2 is active

// Timing constants for Morse code signal interpretation
const int dotLength = 240;       // Duration of a dot (in milliseconds)
const int dashLength = dotLength * 3; // Duration of a dash (3 times the dot length)
const int letterSpace = dotLength * 3; // Space between letters (3 times the dot length)
const int wordSpace = dotLength * 7;   // Space between words (7 times the dot length)

int t1, t2, onTime, gap;  // Timing variables for handling Morse code timing
bool newLetter = false, newWord = false;  // Flags to indicate if a new letter or word has been entered
int lineLength = 0;  // Track the length of the current line for display
const int maxLineLength = 20;  // Maximum number of characters to display per line

// Morse code table for encoding/decoding letters and digits
const char morseCodeTable[36][6] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
  "-----", ".----", "..---", "...--", "....-",
  ".....", "-....", "--...", "---..", "----."
};

char dashSeq[10] = "";  // Buffer to hold the Morse code sequence for each letter
char keyLetter;  // Holds the decoded letter
bool clearTransmitFlag = false;  // Flag to clear the display when transmit mode changes
bool transmitChanged = false;  // Flag to track whether transmit mode has changed

int x = 0;  // Horizontal position on the LCD screen
int y = 0;  // Vertical position on the LCD screen

// Setup function runs once when the program starts
void setup() {
  Serial.begin(115200);  // Start the Serial Monitor for debugging

  // Initialize the LCD screen with 20 columns and 4 rows
  lcd.begin(20, 4);  
  lcd.clear();  // Clear any previous content on the screen
  lcd.setCursor(0, 0);  // Set cursor to the top-left corner
  lcd.print("Morse Code");  // Display the initial message on the LCD

  // PinMode setup for input/output
  pinMode(ledPin, OUTPUT);
  pinMode(tonePin, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);  // Set button pins as input with pull-up resistors
  pinMode(buttonPin2, INPUT_PULLUP);  // Set button pins as input with pull-up resistors
  pinMode(transmitPin1, INPUT_PULLUP);
  pinMode(transmitPin2, INPUT_PULLUP);
  pinMode(transmitLed1, OUTPUT);
  pinMode(transmitLed2, OUTPUT);

  // Serial Monitor instructions
  Serial.println(F("\nMorse Code decoder/encoder"));
  Serial.println(F("-------------------------------"));
  Serial.println(F("Click field in Serial Monitor,"));
  Serial.println(F("type text and press Enter, or"));
  Serial.println(F("Key in Morse Code to decode:"));
  Serial.println(F("-------------------------------"));
}

// Loop function runs repeatedly
void loop() {
  static unsigned long lastDebounceTime1 = 0;
  static unsigned long lastDebounceTime2 = 0;
  static bool lastButtonState1 = HIGH;
  static bool lastButtonState2 = HIGH;

  unsigned long debounceDelay = 50;  // Debounce time for button presses (in milliseconds)

  // Read the state of each transmitter switch with debouncing
  bool currentButtonState1 = digitalRead(transmitPin1);
  bool currentButtonState2 = digitalRead(transmitPin2);

  // Handle debounce for transmitPin1 (transmitter switch 1)
  if (currentButtonState1 != lastButtonState1) {
    lastDebounceTime1 = millis();  // Update debounce time
  }
  if ((millis() - lastDebounceTime1) > debounceDelay) {
    if (currentButtonState1 == LOW && !transmitActive1) {
      if (!transmitActive2) {  // Ensure only one transmit switch is active
        transmitActive1 = true;
        clearTransmitFlag = true;  // Flag to clear display when transmit mode changes
        transmitChanged = true;  // Flag to refresh display
        handleTransmitSwitch(transmitPin1);  // Handle the switch for transmit mode 1
      }
    } else if (currentButtonState1 == HIGH && transmitActive1) {
      transmitActive1 = false;
      clearTransmitFlag = true;
      transmitChanged = true;
      handleTransmitSwitch(transmitPin1);  // Handle the switch for transmit mode 1 deactivation
    }
  }

  // Handle debounce for transmitPin2 (transmitter switch 2)
  if (currentButtonState2 != lastButtonState2) {
    lastDebounceTime2 = millis();  // Update debounce time
  }
  if ((millis() - lastDebounceTime2) > debounceDelay) {
    if (currentButtonState2 == LOW && !transmitActive2) {
      if (!transmitActive1) {  // Ensure only one transmit switch is active
        transmitActive2 = true;
        clearTransmitFlag = true;
        transmitChanged = true;
        handleTransmitSwitch(transmitPin2);  // Handle the switch for transmit mode 2
      }
    } else if (currentButtonState2 == HIGH && transmitActive2) {
      transmitActive2 = false;
      clearTransmitFlag = true;
      transmitChanged = true;
      handleTransmitSwitch(transmitPin2);  // Handle the switch for transmit mode 2 deactivation
    }
  }

  lastButtonState1 = currentButtonState1;
  lastButtonState2 = currentButtonState2;

  // Handle Morse code input from either button, regardless of transmit mode
  if (digitalRead(buttonPin1) == LOW || digitalRead(buttonPin2) == LOW) {
    handleMorseCode();  // Capture Morse code regardless of whether transmission is active
  }

  gap = millis() - t2;  // Calculate the time gap between inputs
  if (newLetter && gap >= letterSpace) {  // If a new letter is detected and timing is correct
    keyLetter = decodeMorse();  // Decode the Morse code sequence into a letter
    Serial.print(keyLetter);  // Print the decoded letter to Serial Monitor for debugging
    if (keyLetter == '?') tone(tonePin, 100, 500);  // Play a sound if decoding fails
    newLetter = false;  // Reset the new letter flag
    dashSeq[0] = '\0';  // Clear the Morse code sequence buffer
    lineLength++;  // Increment the line length

    // Display the decoded letter on the LCD
    displayDecodedChar(keyLetter);
  }

  // Handle spaces between words
  if (newWord && gap >= wordSpace * 1.5) {
    newWord = false;  // Reset new word flag
    Serial.print("_");  // Print an underscore for a space in the Serial Monitor
    lineLength++;

    // Display the space character on the LCD
    displayDecodedChar(' ');
  }

  // If the line reaches the maximum line length, move to the next line
  if (lineLength >= maxLineLength) {
    Serial.println();  // Print a newline in the Serial Monitor
    lineLength = 0;  // Reset line length
  }
}

// Function to handle changes in transmit mode (either Transmit 1 or Transmit 2)
void handleTransmitSwitch(int transmitPin) {
  Serial.print("Transmit switch ");
  Serial.print((transmitPin == transmitPin1) ? "1" : "2");
  Serial.println(" activated.");

  // Activate the corresponding transmit LED and deactivate the other one
  digitalWrite(transmitLed1, (transmitPin == transmitPin1) ? HIGH : LOW);
  digitalWrite(transmitLed2, (transmitPin == transmitPin2) ? HIGH : LOW);

  // Deactivate the other transmitter switch pin
  if (transmitPin == transmitPin1) {
    digitalWrite(transmitPin2, HIGH);  // Disable Transmit 2
  } else {
    digitalWrite(transmitPin1, HIGH);  // Disable Transmit 1
  }

  if (transmitChanged) {
    lcd.clear();  // Clear the LCD display
    x = 0;
    y = 0;
    transmitChanged = false;  // Reset transmit changed flag
  }

  lcd.setCursor(0, 0);  // Set cursor to the top-left corner
  lcd.print("Transmit ");
  lcd.print((transmitPin == transmitPin1) ? "1" : "2");  // Display the active transmit mode
}

// Function to handle Morse code input (dot or dash)
void handleMorseCode() {
  int buttonPin = (transmitActive1 && digitalRead(buttonPin1) == LOW) ? buttonPin1 : buttonPin2;

  if (clearTransmitFlag) {
    lcd.setCursor(0, 0);
    lcd.clear();  // Clear the display when transmit mode changes
    clearTransmitFlag = false;
  }

  newLetter = true;
  newWord = true;
  t1 = millis();  // Record the time when the button was pressed
  digitalWrite(ledPin, HIGH);  // Turn on the LED to indicate input is being registered
  tone(tonePin, 1000);  // Play a tone for visual and auditory feedback
  delay(30);  // Short delay for button press
  while (digitalRead(buttonPin) == LOW) delay(30);  // Wait for the button to be released
  delay(30);

  t2 = millis();  // Record the time when the button was released
  onTime = t2 - t1;  // Calculate the duration of the button press
  digitalWrite(ledPin, LOW);  // Turn off the LED after input
  noTone(tonePin);  // Stop the tone when button is released

  // If the button press duration is short, consider it a dot; otherwise, it's a dash
  if (onTime <= dotLength * 1.5) {
    strcat(dashSeq, ".");  // Add dot to the Morse code sequence
  } else {
    strcat(dashSeq, "-");  // Add dash to the Morse code sequence
  }
}

// Function to decode a Morse code sequence into a letter or digit
char decodeMorse() {
  for (int i = 0; i < 36; i++) {
    if (strcmp(dashSeq, morseCodeTable[i]) == 0) {  // Check if the Morse code matches any letter/digit
      return (i < 26) ? (char)('A' + i) : (char)('0' + (i - 26));  // Return corresponding letter or digit
    }
  }
  return '?';  // Return '?' if the Morse code is invalid
}

// Function to display a decoded character on the LCD
void displayDecodedChar(char decodedChar) {
  lcd.setCursor(x, y);  // Set the LCD cursor position
  lcd.print(decodedChar);  // Print the decoded character to the LCD

  x += 1;  // Move the cursor to the next position horizontally
  if (x >= 20) {  // If the end of the line is reached
    x = 0;  // Reset to the first column
    y++;  // Move to the next row
    if (y >= 4) {  // If the end of the screen is reached
      y = 0;  // Reset to the first row
      lcd.clear();  // Clear the LCD screen
    }
  }
}
