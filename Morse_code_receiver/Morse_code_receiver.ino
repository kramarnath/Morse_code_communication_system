/* ------------------------------------------------------------
   MORSE CODE RECEIVER (ESP32 + LASER RECEIVER)
   ------------------------------------------------------------ */

#include <LiquidCrystal.h>

// LCD pins
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

// Receiver pins
const int sensorPin = 34;     // LDR / Photodiode
const int buzzerPin = 15;

// Timing (ms) — keep in sync with transmitter
const int dotTime      = 200;
const int dashTime     = 600;
const int letterGapMs  = 800;   // silence longer than this = new letter

// Classification threshold: midpoint between dot and dash durations
const unsigned long thresholdMs = (dotTime + dashTime) / 2;   // 400 ms

// Hysteresis: sensor must cross these bands to change state (avoids chatter)
// thresholdValue is set at runtime during auto-calibration
int thresholdValue   = 1500;    // overwritten in setup()
const int hysteresis = 150;     // counts above/below baseline to flip state

// Pulse tracking
unsigned long pulseStart    = 0;
unsigned long lastPulseEnd  = 0;
bool          pulseActive   = false;

// Symbol buffer — longest Morse is 6 chars
char morseSymbol[8] = {0};
int  symbolIdx      = 0;

// Display buffers
char morseDisplay[13] = {0};    // raw morse of current symbol (LCD row 1)
int  morseDisplayLen  = 0;
char message[13]      = {0};    // decoded message (LCD row 0)
int  messageLen       = 0;

// ---------------------------------------------------------------
// Morse decode lookup — A-Z and 0-9
// ---------------------------------------------------------------
char decodeMorse(const char* code) {
  struct { const char* morse; char letter; } table[] = {
    {".-",    'A'}, {"-...",  'B'}, {"-.-.",  'C'}, {"-..",   'D'},
    {".",     'E'}, {"..-.",  'F'}, {"--.",   'G'}, {"....",  'H'},
    {"..",    'I'}, {".---",  'J'}, {"-.-",   'K'}, {".-..",  'L'},
    {"--",    'M'}, {"-.",    'N'}, {"---",   'O'}, {".--.",  'P'},
    {"--.-",  'Q'}, {".-.",   'R'}, {"...",   'S'}, {"-",     'T'},
    {"..-",   'U'}, {"...-",  'V'}, {".--",   'W'}, {"-..-",  'X'},
    {"-.--",  'Y'}, {"--..",  'Z'},
    {"-----", '0'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'},
    {"....-", '4'}, {".....", '5'}, {"-...." ,'6'}, {"--...", '7'},
    {"---..", '8'}, {"----.", '9'}
  };
  int tableSize = sizeof(table) / sizeof(table[0]);
  for (int i = 0; i < tableSize; i++) {
    if (strcmp(code, table[i].morse) == 0) return table[i].letter;
  }
  return '?';
}

// ---------------------------------------------------------------
// Auto-calibrate: sample the sensor with laser OFF to get the
// ambient baseline, then set threshold above that baseline
// ---------------------------------------------------------------
void calibrate() {
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...  ");
  Serial.println("Calibrating sensor baseline...");

  long sum = 0;
  const int samples = 100;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(sensorPin);
    delay(10);
  }
  int baseline = sum / samples;
  thresholdValue = baseline + 300;   // 300 counts of margin above dark baseline

  Serial.print("Baseline: ");
  Serial.print(baseline);
  Serial.print(" | Threshold set to: ");
  Serial.println(thresholdValue);

  lcd.setCursor(0, 0);
  lcd.print("Cal done        ");
  delay(800);
}

void setup() {
  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse RX Ready");
  delay(1500);
  lcd.clear();

  calibrate();   // auto-set threshold from ambient reading

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MSG:");
  lcd.setCursor(0, 1);
  lcd.print("SYM:");

  Serial.println("Morse Receiver Ready");
}

void loop() {
  int sensorValue        = analogRead(sensorPin);
  unsigned long now      = millis();

  // Laser ON detected — upper hysteresis band
  if (sensorValue > (thresholdValue + hysteresis) && !pulseActive) {
    pulseActive = true;
    pulseStart  = now;
    digitalWrite(buzzerPin, HIGH);

    Serial.print("Laser ON  | sensor: ");
    Serial.println(sensorValue);
  }

  // Laser OFF detected — lower hysteresis band
  if (sensorValue < (thresholdValue - hysteresis) && pulseActive) {
    unsigned long pulseDuration = now - pulseStart;
    pulseActive  = false;
    lastPulseEnd = now;
    digitalWrite(buzzerPin, LOW);

    Serial.print("Laser OFF | duration: ");
    Serial.print(pulseDuration);
    Serial.print(" ms  -> ");

    // Classify pulse as dot or dash using calibrated midpoint
    char symbol = (pulseDuration < thresholdMs) ? '.' : '-';

    Serial.println(symbol);

    appendToSymbol(symbol);
    updateLCD();
  }

  // Letter-gap timeout — if laser has been off for longer than letterGapMs,
  // the current symbol is complete: decode it and start a new one
  if (!pulseActive && symbolIdx > 0 && (now - lastPulseEnd) > letterGapMs) {
    char decoded = decodeMorse(morseSymbol);

    Serial.print("LETTER | morse: ");
    Serial.print(morseSymbol);
    Serial.print("  ->  '");
    Serial.print(decoded);
    Serial.println("'");

    // Append to message, scroll left if full
    if (messageLen < 12) {
      message[messageLen++] = decoded;
      message[messageLen]   = '\0';
    } else {
      memmove(message, message + 1, 11);
      message[11] = decoded;
      message[12] = '\0';
    }

    // Reset symbol buffers
    memset(morseSymbol,  0, sizeof(morseSymbol));
    symbolIdx = 0;
    memset(morseDisplay, 0, sizeof(morseDisplay));
    morseDisplayLen = 0;

    // Reset lastPulseEnd so this block doesn't fire again immediately
    lastPulseEnd = now - letterGapMs - 1;

    updateLCD();
  }
}

// ---------------------------------------------------------------
// Append a dot/dash to the symbol and display buffers
// ---------------------------------------------------------------
void appendToSymbol(char c) {
  if (symbolIdx < 6) {
    morseSymbol[symbolIdx++] = c;
    morseSymbol[symbolIdx]   = '\0';
  }

  if (morseDisplayLen < 12) {
    morseDisplay[morseDisplayLen++] = c;
    morseDisplay[morseDisplayLen]   = '\0';
  } else {
    memmove(morseDisplay, morseDisplay + 1, 11);
    morseDisplay[11] = c;
    morseDisplay[12] = '\0';
  }
}

// ---------------------------------------------------------------
// Refresh both LCD rows
// Row 0: "MSG:" + decoded message
// Row 1: "SYM:" + raw dots/dashes of current symbol
// ---------------------------------------------------------------
void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("MSG:");
  lcd.setCursor(4, 0);
  lcd.print(message);
  for (int i = messageLen; i < 12; i++) lcd.print(' ');

  lcd.setCursor(0, 1);
  lcd.print("SYM:");
  lcd.setCursor(4, 1);
  lcd.print(morseDisplay);
  for (int i = morseDisplayLen; i < 12; i++) lcd.print(' ');
}
