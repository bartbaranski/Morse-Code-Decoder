#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzerPin = 3;
const int buttonPin1 = 2;
const int buttonPin2 = 4;
const int buttonPin3 = 6; // Dodajemy trzeci przycisk do resetowania LCD
const int ledPin = 5;

boolean buttonPressed = false;
boolean useSerial = false;
String morseCode = "";
String translatedText = "";
String lastDisplayedText = "";
unsigned long pressStartTime = 0;
unsigned long lastPressTime = 0;
const unsigned long dotLength = 200;
const unsigned long dashLength = 600;
const unsigned long symbolTimeout = 1000;
unsigned long lastCharTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP); // Ustawiamy trzeci przycisk jako wejście
  pinMode(ledPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse Code:");
  lcd.setCursor(0, 1);
}

void loop() {
  // Sprawdź, czy przycisk zmiany trybu został wciśnięty
  if (digitalRead(buttonPin2) == LOW) {
    useSerial = !useSerial;
    delay(300);
    lcd.clear();
    lcd.setCursor(0, 0);
    if (useSerial) {
      lcd.print("Serial Mode");
    } else {
      lcd.print("Button Mode");
    }
    lcd.setCursor(0, 1);
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Morse Code:");
    lcd.setCursor(0, 1);
    translatedText = "";
    morseCode = "";
    lastDisplayedText = "";
  }

  // Sprawdź, czy trzeci przycisk został wciśnięty
  if (digitalRead(buttonPin3) == LOW) {
    morseCode = "";
    translatedText = "";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Morse Code:");
    lcd.setCursor(0, 1);
    delay(300); // debounce
  }

  if (useSerial) {
    readSerial();
  } else {
    readButton();
  }

  // Sprawdzanie, czy upłynął czas symbolTimeout bez sygnału
  if (!useSerial && millis() - lastPressTime > symbolTimeout && morseCode.length() > 0) {
    translateAndDisplay();
  }

  // Miga dioda LED, gdy wiadomość z komputera czeka na przyjęcie
  static unsigned long previousMillis = 0;
  const long interval = 500;
  if (Serial.available() > 0 && !useSerial) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void readButton() {
  if (digitalRead(buttonPin1) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      digitalWrite(buzzerPin, HIGH);
      pressStartTime = millis();
    }
  } else {
    if (buttonPressed) {
      unsigned long pressEndTime = millis();
      unsigned long duration = pressEndTime - pressStartTime;
      digitalWrite(buzzerPin, LOW);
      buttonPressed = false;

      Serial.print("Duration: ");
      Serial.println(duration);

      if (duration < dotLength) {
        morseCode += ".";
        Serial.println("Dot");
      } else if (duration < dashLength) {
        morseCode += "-";
        Serial.println("Dash");
      } else {
        Serial.println("Too long");
      }

      lastPressTime = millis();
      updateLCD(translatedText + morseCode);
    }
  }
}

void readSerial() {
  static bool newMessage = true;
  static const unsigned long charTimeout = 500;

  if (Serial.available()) {
    if (newMessage) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Morse Code:");
      lcd.setCursor(0, 1);
      translatedText = "";
      morseCode = "";
      lastDisplayedText = "";
      newMessage = false;
    }
    
    char c = Serial.read();
    if (c == '.') {
      morseCode += ".";
      dot();
    } else if (c == '-') {
      morseCode += "-";
      dash();
    } else if (c == ' ') {
      translateAndDisplay();
    } else if (c == '/') {
      translateAndDisplay();
      translatedText += ' ';
    }
    lastCharTime = millis();
  }

  if (millis() - lastCharTime > charTimeout && morseCode.length() > 0) {
    translateAndDisplay();
    lastCharTime = millis();
    newMessage = true;
  }

  updateLCD(translatedText + morseCode);
}

void dot() {
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  delay(200);
}

void dash() {
  digitalWrite(buzzerPin, HIGH);
  delay(600);
  digitalWrite(buzzerPin, LOW);
  delay(200);
}

void translateAndDisplay() {
  String letter = morseToLetter(morseCode);
  translatedText += letter;
  morseCode = "";
  updateLCD(translatedText);
  Serial.print("Translated: ");
  Serial.println(translatedText);
}

void updateLCD(String text) {
  if (text != lastDisplayedText) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Morse Code:");
    lcd.setCursor(0, 1);
    lcd.print(text);
    lastDisplayedText = text;
  }
}

String morseToLetter(String code) {
  if (code == ".-") return "A";
  if (code == "-...") return "B";
  if (code == "-.-.") return "C";
  if (code == "-..") return "D";
  if (code == ".") return "E";
  if (code == "..-.") return "F";
  if (code == "--.") return "G";
  if (code == "....") return "H";
  if (code == "..") return "I";
  if (code == ".---") return "J";
  if (code == "-.-") return "K";
  if (code == ".-..") return "L";
  if (code == "--") return "M";
  if (code == "-.") return "N";
  if (code == "---") return "O";
  if (code == ".--.") return "P";
  if (code == "--.-") return "Q";
  if (code == ".-.") return "R";
  if (code == "...") return "S";
  if (code == "-") return "T";
  if (code == "..-") return "U";
  if (code == "...-") return "V";
  if (code == ".--") return "W";
  if (code == "-..-") return "X";
  if (code == "-.--") return "Y";
  if (code == "--..") return "Z";
  if (code == "-----") return "0";
  if (code == ".----") return "1";
  if (code == "..---") return "2";
  if (code == "...--") return "3";
  if (code == "....-") return "4";
  if (code == ".....") return "5";
  if (code == "-....") return "6";
  if (code == "--...") return "7";
  if (code == "---..") return "8";
  if (code == "----.") return "9";
  return ""; // Spacja dla nieznanego kodu Morse'a
}
