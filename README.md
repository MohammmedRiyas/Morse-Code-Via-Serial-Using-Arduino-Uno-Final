# Morse Code Transmitter and Receiver with LCD Display  

## Overview  
This Arduino-based Morse Code transmitter and receiver system enables real-time communication via Morse code. It features an intuitive interface using a **20x4 LCD display**, buttons for input, and **LED/buzzer feedback**. The system supports **two-way communication**, ensuring only one transmission mode is active at a time.  

## Features  
- 🛜 **Two Distinct Transmit Modes**: Supports `Transmit 1` and `Transmit 2`, allowing only one to be active at a time.  
- 🔄 **Real-time Morse Code Encoding & Decoding**: Instantly displays transmitted/received characters.  
- 🔔 **Visual & Audio Feedback**: LED indicators and buzzer provide real-time user feedback.  
- 📟 **20x4 LCD Display**: Shows transmitted and received Morse code characters.  
- 🔁 **Automatic Line Wrapping & Clearing**: Ensures continuous use without clutter.  
- ⚡ **Debouncing Logic**: Prevents false triggers from mechanical button noise.  

---

## Hardware Requirements  
🔧 Components Needed:  
1. **Arduino Uno**  
2. **20x4 LCD with I2C Interface**  
3. **Push Buttons (x2)** – For Morse code input  
4. **Switches (x2)** – To control transmit modes  
5. **LEDs (x3)** – Two for transmit mode indicators, one for input feedback  
6. **Buzzer** – For audio feedback  
7. **Jumper Wires**  

---

## 📜 Circuit Diagram  
*(To be added: Include an image of your circuit diagram here.)*  

---

## 🛠 How It Works  

### 🎛 **Debouncing Logic**  
To eliminate false triggers due to mechanical noise in button presses, we use **millis()** for debouncing:  

```cpp
if ((millis() - lastDebounceTime) > debounceDelay) {
  // Button state change logic
}
```
- **Debounce Delay**: A **50ms delay** ensures the button state is stable before processing.  
- **Logic**: When a button state changes, the system records the timestamp and confirms it only if stable for the debounce delay.  

---

### 📡 **Transmit and Receive Modes**  
- The system supports **two distinct transmit modes** (`Transmit 1` & `Transmit 2`).  
- Activating one **disables the other** and **clears the LCD**.  
- **LED indicators** light up to show the active mode.  

---

### 🔠 **Morse Code Decoding**  
- **Dot (.)**: Button press duration ≤ **1.5x dot length**  
- **Dash (-)**: Button press duration > **1.5x dot length**  
- **Letter Gap**: 3x **dot length**  
- **Word Gap**: 7x **dot length**  

Characters are displayed on the **LCD screen in real-time**.  

---

## 📜 Code Explanation  

### 🔹 **Key Sections**  
1. **`setup()` Function**  
   - Initializes **LCD, pins, and Serial Monitor**.  
   - Displays an introductory message.  

2. **`loop()` Function**  
   - Handles **debouncing for transmitter buttons**.  
   - Processes **Morse code input**.  
   - Updates the **LCD display** accordingly.  

3. **Helper Functions**  
   - `handleTransmitSwitch()`: Manages activation of transmit modes.  
   - `handleMorseCode()`: Captures Morse code input.  
   - `decodeMorse()`: Converts Morse code sequences into characters.  
   - `displayDecodedChar()`: Displays decoded characters on the **LCD**.  

---

### ⏳ **Timing Using millis()**  
Instead of `delay()`, we use `millis()` for non-blocking operations:  
- **Debouncing** – Ensures reliable button detection.  
- **Morse Code Timing** – Differentiates **dots, dashes, letters, and words**.  
- **LCD Display Updates** – Clears or updates display when needed.  

---

## 💡 **Uniqueness of the Project**  
✅ **Two-Way Communication**: Seamless switching between two transmitters.  
✅ **Real-Time Decoding**: Instant character display.  
✅ **Debouncing Logic**: Ensures accurate button input detection.  
✅ **Dynamic LCD Handling**: Automatically clears or wraps text when full.  
✅ **Error Handling**: Plays a tone for invalid Morse sequences.  

---

## 🛠 **How to Use**  
1. **Assemble** the components as per the circuit diagram.  
2. **Upload the code** to the Arduino using the **Arduino IDE**.  
3. **Power On** the system.  
4. **Select a transmit mode** using the switches.  
5. **Input Morse Code** using the respective button.  
6. **View the decoded characters** on the LCD screen.  

---

## 📜 Done By  
👤 **Mohammed Riyas N**  
🏫 **Bannari Amman Institute of Technology**  

---

### 📌 *If you find this project useful, don't forget to ⭐ this repository!*  

---
