/* ------------------------------------------------------------
   MORSE CODE TRANSMITTER (ESP32 + LASER TRANSMITTER MODULE)
   ------------------------------------------------------------ */

#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

// Buttons for dot and dash
const int dotButton = 4;
const int dashButton = 5;

// Laser output pin
const int laserPin = 2;

String morseString = "";

// Morse timing (ms)
const int dotTime = 200;
const int dashTime = 600;
const int symbolGap = 200;
const int letterGap = 800;

void setup() {
  pinMode(dotButton, INPUT_PULLUP);
  pinMode(dashButton, INPUT_PULLUP);
  pinMode(laserPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse TX Ready");
  delay(1500);
  lcd.clear();
}

void loop() {

  // DOT
  if (digitalRead(dotButton) == LOW) {
    sendDot();
    delay(300);
  }

  // DASH
  if (digitalRead(dashButton) == LOW) {
    sendDash();
    delay(300);
  }
}

void sendDot() {
  morseString += ".";
  lcd.setCursor(0, 1);
  lcd.print(morseString);

  digitalWrite(laserPin, HIGH);
  delay(dotTime);
  digitalWrite(laserPin, LOW);
  delay(symbolGap);
}

void sendDash() {
  morseString += "-";
  lcd.setCursor(0, 1);
  lcd.print(morseString);

  digitalWrite(laserPin, HIGH);
  delay(dashTime);
  digitalWrite(laserPin, LOW);
  delay(symbolGap);
}
