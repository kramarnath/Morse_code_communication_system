/* ------------------------------------------------------------
   MORSE CODE RECEIVER (ESP32 + LASER RECEIVER)
   ------------------------------------------------------------ */

#include <LiquidCrystal.h>

// LCD pins
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

// Receiver pins
const int sensorPin = 34;     // LDR / Photodiode
const int buzzerPin = 15;

// Threshold for detecting light from laser
int thresholdValue = 1500;

unsigned long pulseStart = 0;
bool pulseActive = false;

String morse = "";

void setup() {
  pinMode(buzzerPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse RX Ready");
  delay(1500);
  lcd.clear();
}

void loop() {

  int sensorValue = analogRead(sensorPin);

  // Laser ON detected
  if (sensorValue > thresholdValue && !pulseActive) {
    pulseActive = true;
    pulseStart = millis();
    digitalWrite(buzzerPin, HIGH);
  }

  // Laser OFF detected
  if (sensorValue < thresholdValue && pulseActive) {
    unsigned long pulseDuration = millis() - pulseStart;
    pulseActive = false;
    digitalWrite(buzzerPin, LOW);

    if (pulseDuration < 350) {
      morse += ".";
    } else {
      morse += "-";
    }

    lcd.setCursor(0, 1);
    lcd.print(morse);
  }
}
