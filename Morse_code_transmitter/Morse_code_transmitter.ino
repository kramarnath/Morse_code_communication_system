/* ------------------------------------------------------------
   MORSE CODE TRANSMITTER (ESP32 + LASER TRANSMITTER MODULE)
   ------------------------------------------------------------ */

#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

// Buttons for dot, dash, and letter-gap
const int dotButton    = 4;
const int dashButton   = 5;
const int letterButton = 18;    // NEW: press to mark end of a letter

// Laser output pin
const int laserPin  = 2;

// Buzzer pin
const int buzzerPin = 17;

// Morse timing (ms)
const int dotTime   = 200;
const int dashTime  = 600;
const int symbolGap = 200;
const int letterGap = 800;

// Midpoint used to classify incoming pulse on receiver side
// Keep in sync with receiver: THRESHOLD_MS = (dotTime + dashTime) / 2 = 400
const int thresholdMs = 400;

// Debounce delay (ms)
const unsigned long debounceDelay = 50;

// Debounce timestamps
unsigned long lastDotTime    = 0;
unsigned long lastDashTime   = 0;
unsigned long lastLetterTime = 0;

// Current symbol buffer — longest Morse symbol is 6 chars (e.g. "......")
char morseSymbol[8] = {0};
int  symbolIdx      = 0;

// Decoded message shown on LCD row 0 (max 12 chars after "MSG:" prefix)
char message[13] = {0};
int  messageLen  = 0;

// Raw dots/dashes shown on LCD row 1 (max 12 chars after "SYM:" prefix)
char morseDisplay[13] = {0};
int  morseDisplayLen  = 0;

void setup() {
  Serial.begin(115200);

  pinMode(dotButton,    INPUT_PULLUP);
  pinMode(dashButton,   INPUT_PULLUP);
  pinMode(letterButton, INPUT_PULLUP);
  pinMode(laserPin,     OUTPUT);
  pinMode(buzzerPin,    OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse TX Ready");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MSG:");
  lcd.setCursor(0, 1);
  lcd.print("SYM:");

  Serial.println("Morse Transmitter Ready");
}

void loop() {
  unsigned long now = millis();

  // DOT button — debounced
  if (digitalRead(dotButton) == LOW && (now - lastDotTime) > debounceDelay) {
    lastDotTime = now;
    sendDot();
  }

  // DASH button — debounced
  if (digitalRead(dashButton) == LOW && (now - lastDashTime) > debounceDelay) {
    lastDashTime = now;
    sendDash();
  }

  // LETTER button — signal letter gap, decode symbol, advance message
  if (digitalRead(letterButton) == LOW && (now - lastLetterTime) > debounceDelay) {
    lastLetterTime = now;
    sendLetterGap();
  }
}

// ---------------------------------------------------------------
// Transmit a dot pulse and append to display buffer
// ---------------------------------------------------------------
void sendDot() {
  appendToSymbol('.');

  Serial.print("DOT  | current symbol: ");
  Serial.println(morseSymbol);

  digitalWrite(laserPin,  HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(dotTime);
  digitalWrite(laserPin,  LOW);
  digitalWrite(buzzerPin, LOW);
  delay(symbolGap);

  updateLCD();
}

// ---------------------------------------------------------------
// Transmit a dash pulse and append to display buffer
// ---------------------------------------------------------------
void sendDash() {
  appendToSymbol('-');

  Serial.print("DASH | current symbol: ");
  Serial.println(morseSymbol);

  digitalWrite(laserPin,  HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(dashTime);
  digitalWrite(laserPin,  LOW);
  digitalWrite(buzzerPin, LOW);
  delay(symbolGap);

  updateLCD();
}

// ---------------------------------------------------------------
// Send the inter-letter gap (silence), decode the buffered symbol
// and append the result to the message on LCD row 0
// ---------------------------------------------------------------
void sendLetterGap() {
  if (symbolIdx == 0) return;   // nothing buffered, ignore

  // Hold laser LOW for letter-gap duration so receiver knows letter ended
  delay(letterGap);

  char decoded = decodeMorse(morseSymbol);

  Serial.print("LETTER | morse: ");
  Serial.print(morseSymbol);
  Serial.print("  ->  '");
  Serial.print(decoded);
  Serial.println("'");

  // Confirmation double-beep so the sender hears each letter was sent
  digitalWrite(buzzerPin, HIGH); delay(80);
  digitalWrite(buzzerPin, LOW);  delay(80);
  digitalWrite(buzzerPin, HIGH); delay(80);
  digitalWrite(buzzerPin, LOW);

  // Append decoded char — scroll left if row is full
  if (messageLen < 12) {
    message[messageLen++] = decoded;
    message[messageLen]   = '\0';
  } else {
    memmove(message, message + 1, 11);
    message[11] = decoded;
    message[12] = '\0';
  }

  // Clear symbol buffers for next letter
  memset(morseSymbol,   0, sizeof(morseSymbol));
  symbolIdx = 0;
  memset(morseDisplay,  0, sizeof(morseDisplay));
  morseDisplayLen = 0;

  updateLCD();
}

// ---------------------------------------------------------------
// Append a dot/dash character to both symbol and display buffers
// ---------------------------------------------------------------
void appendToSymbol(char c) {
  // Symbol buffer — cap at 6 elements (longest valid Morse code)
  if (symbolIdx < 6) {
    morseSymbol[symbolIdx++] = c;
    morseSymbol[symbolIdx]   = '\0';
  }

  // Display buffer — scroll left to always show latest 12 chars
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
// Row 0: "MSG:" + decoded message (scrolls left)
// Row 1: "SYM:" + raw morse of current symbol (scrolls left)
// ---------------------------------------------------------------
void updateLCD() {
  // Row 0 — decoded message
  lcd.setCursor(0, 0);
  lcd.print("MSG:");
  lcd.setCursor(4, 0);
  lcd.print(message);
  for (int i = messageLen; i < 12; i++) lcd.print(' ');

  // Row 1 — raw morse symbol being built
  lcd.setCursor(0, 1);
  lcd.print("SYM:");
  lcd.setCursor(4, 1);
  lcd.print(morseDisplay);
  for (int i = morseDisplayLen; i < 12; i++) lcd.print(' ');
}

// ---------------------------------------------------------------
// Morse decode lookup — A-Z and 0-9
// Returns '?' for unrecognised symbols
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
