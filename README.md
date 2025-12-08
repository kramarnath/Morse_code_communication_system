
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

## 📸 Images
Place your TX/RX images in the `/Images` folder.

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
