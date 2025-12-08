# Morse Code Communication System using ESP32 + Laser

A wireless optical communication system implemented using two ESP32 microcontrollers, a laser transmitter, a laser receiver (LDR/photodiode module), and LCD displays.

The system converts button input into Morse code, transmits pulses through a laser beam, receives and decodes the pulses, and displays the output on LCD.

---

## 🚀 Features
- Dot and dash input using buttons  
- Morse code encoding on ESP32  
- Laser-based optical transmission  
- LDR/photodiode-based reception  
- LCD output on both TX and RX sides  
- Buzzer feedback on receiver  
- Simple, reliable and low-cost communication  

---

## 🧰 Hardware Used
- ESP32 Dev Boards (2)  
- Laser Transmitter Module  
- Laser Receiver (LDR / Photodiode sensor)  
- 16x2 LCD Display  
- Push Buttons  
- Buzzer  
- Power Supply / Battery  
- Resistors & Jumper wires  

---

## 📡 System Overview

### **Transmitter**
- User presses DOT (.) or DASH (-) buttons  
- ESP32 stores Morse pattern  
- Laser outputs timed ON/OFF pulses  
- LCD displays entered Morse symbols  

### **Receiver**
- LDR/photodiode detects laser pulses  
- ESP32 calculates pulse duration  
- Short pulse = DOT, long pulse = DASH  
- LCD displays the decoded Morse  
- Buzzer beeps on each received pulse  

---

## 🔧 Connections

### **Transmitter**
| Component | ESP32 Pin |
|----------|-----------|
| Dot Button | 4 |
| Dash Button | 5 |
| Laser Module | 2 |
| LCD | GPIO 13,12,14,27,26,25 |

### **Receiver**
| Component | ESP32 Pin |
|----------|-----------|
| LDR/Photodiode | 34 |
| Buzzer | 15 |
| LCD | GPIO 13,12,14,27,26,25 |

---

## ▶️ How to Use
### **Transmitter**
1. Press DOT or DASH buttons  
2. Laser flashes dot/dash pulses  
3. LCD shows entered code  

### **Receiver**
1. Align the laser to the LDR  
2. Receiver detects flashes  
3. LCD shows Morse code  
4. Buzzer beeps for each pulse

---

## 🛠 Improvements (Future Work)
- Alphabet decoding  
- Wireless serial display  
- Higher-range laser  
- WiFi/Bluetooth Morse messaging  
- Error correction  

---

## 📜 License
This project is open-source. You may use and modify it with credit.
