# Morse Code Communication System using ESP32 + Laser

A wireless morse code communication system implemented using two ESP32 microcontrollers, a laser transmitter, a laser receiver (LDR/photodiode module), and LCD displays. The system converts button input into Morse code, transmits pulses through a laser beam, receives and decodes the pulses into alphabet characters, and displays the output on LCD.

---

## Features
- Dot and dash input using buttons
- Dedicated letter-gap button to signal end of each character
- Morse code encoding on ESP32
- Laser-based optical transmission
- LDR/photodiode-based reception with auto-calibration
- Hysteresis on the sensor threshold to prevent chatter
- Full Morse decode (A–Z, 0–9) on both TX and RX sides
- Automatic letter-gap timeout on receiver for hands-free decoding
- Scrolling LCD output on both TX and RX sides (no clipping)
- Buzzer feedback on receiver
- Serial debug output on both boards (115200 baud)
- Simple, reliable and low-cost communication

---

## Hardware Used
- ESP32 Dev Boards (2)
- Laser Transmitter Module
- Laser Receiver (LDR / Photodiode sensor)
- 16x2 LCD Display
- Push Buttons (3 on TX side, 2 on RX side)
- Buzzer
- Power Supply / Battery
- Resistors & Jumper wires

---

## 📡 System Overview

### **Transmitter**
- User presses DOT (.) or DASH (-) buttons
- ESP32 stores Morse pattern in a symbol buffer
- Laser outputs timed ON/OFF pulses
- LCD row 1 shows current symbol being entered (scrolls if long)
- User presses the LETTER button when a character is complete
- ESP32 fires a letter-gap pause and decodes the symbol
- LCD row 0 shows the decoded message (scrolls left as message grows)

### **Receiver**
- On startup, the receiver auto-calibrates the LDR threshold from ambient light
- LDR/photodiode detects laser pulses with hysteresis to avoid chatter
- ESP32 calculates pulse duration; short pulse = DOT, long pulse = DASH
- Dot/dash midpoint = 400 ms (matches transmitter timings)
- After 800 ms of silence the receiver automatically decodes the buffered symbol
- LCD row 0 shows decoded message; LCD row 1 shows raw dots/dashes
- Buzzer beeps on each received pulse

---

## Connections

### **Transmitter**
| Component      | ESP32 Pin              |
|----------------|------------------------|
| Dot Button     | 4                      |
| Dash Button    | 5                      |
| Letter Button  | 18 *(new)*             |
| Laser Module   | 2                      |
| LCD            | GPIO 13, 12, 14, 27, 26, 25 |

### **Receiver**
| Component         | ESP32 Pin              |
|-------------------|------------------------|
| LDR / Photodiode  | 34                     |
| Buzzer            | 15                     |
| LCD               | GPIO 13, 12, 14, 27, 26, 25 |

---

## How to Use

### **Transmitter**
1. Power on — LCD shows "Morse TX Ready" then switches to MSG/SYM view
2. Press DOT or DASH buttons to build a Morse symbol
3. Laser flashes the corresponding pulse; LCD row 1 shows the symbol
4. Press the LETTER button when the character is complete
5. The decoded letter appears on LCD row 0 and the receiver auto-decodes it after the letter-gap pause

### **Receiver**
1. Power on — the board auto-calibrates the LDR threshold from ambient light
2. Align the laser to the LDR
3. Receiver detects flashes and classifies them as dots or dashes
4. After 800 ms of silence the current symbol is decoded automatically
5. LCD row 0 shows the decoded message; LCD row 1 shows the raw Morse
6. Buzzer beeps for each received pulse
7. Open the Arduino Serial Monitor at 115200 baud to see detailed debug output on both boards

---

## Key Timing Constants
| Parameter        | Value  | Notes                                      |
|------------------|--------|--------------------------------------------|
| Dot duration     | 200 ms | Laser ON for a dot                         |
| Dash duration    | 600 ms | Laser ON for a dash                        |
| Symbol gap       | 200 ms | Silence between dots/dashes in one letter  |
| Letter gap       | 800 ms | Silence that signals end of a letter       |
| Decode threshold | 400 ms | Midpoint — pulses below = dot, above = dash |
| Hysteresis       | ±150   | Sensor counts above/below threshold to flip state |

All timing constants are defined at the top of each `.ino` file and kept in sync between TX and RX.

---

## 🛠 Improvements Made
- **Letter-gap button (TX):** Third button on GPIO 18 lets the transmitter explicitly signal the end of each character
- **Morse decode (TX + RX):** Full A–Z and 0–9 lookup table on both sides; decoded text shown on LCD row 0
- **Auto-calibration (RX):** Samples 100 ambient readings on startup to set the LDR threshold dynamically
- **Hysteresis (RX):** Upper and lower threshold bands prevent chattering when the sensor value sits near the boundary
- **Calibrated midpoint (RX):** Dot/dash classification uses the mathematically correct midpoint (400 ms) instead of a manually guessed value
- **LCD scrolling (TX + RX):** Both rows scroll left so long messages never clip off the display
- **Fixed char buffers:** Replaced Arduino `String` with fixed `char[]` arrays to eliminate heap fragmentation on long sessions
- **Debounced buttons (TX):** `millis()`-based debounce on all three buttons prevents ghost presses
- **Serial debug (TX + RX):** `Serial.begin(115200)` with timestamped print statements on both boards for easy debugging

---

## 🛠 Future Work
- Wireless (WiFi / Bluetooth) Morse messaging
- Higher-range laser module
- Error correction / parity check
- Word-gap detection and space character support
- Mobile app integration
