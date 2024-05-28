#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzerPin = 3;
const int buttonPin1 = 2;
const int buttonPin2 = 4;
const int micPin = A0;

boolean buttonPressed = false;
boolean useMicrophone = false;
String morseCode = "";
String translatedText = "";
unsigned long pressStartTime = 0;
unsigned long lastPressTime = 0;
const unsigned long dotLength = 200;
const unsigned long dashLength = 600;
const unsigned long symbolTimeout = 1000; // czas bezczynności oznaczający koniec symbolu
const unsigned long resetThreshold = 2000; // długie naciśnięcie resetuje pamięć
const int micThreshold = 700; // próg wykrywania sygnału mikrofonu

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Morse Code:");
  lcd.setCursor(0, 1);
}

void loop() {
  if (digitalRead(buttonPin2) == LOW) {
    useMicrophone = !useMicrophone;
    delay(300); // debounce
    lcd.clear();
    lcd.setCursor(0, 0);
    if (useMicrophone) {
      lcd.print("Microphone Mode");
    } else {
      lcd.print("Button Mode");
    }
    lcd.setCursor(0, 1);
    delay(1000); // display mode change for 1 second
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Morse Code:");
    lcd.setCursor(0, 1);
    translatedText = "";
  }

  if (useMicrophone) {
    readMicrophone();
  } else {
    readButton();
  }

  // Sprawdzanie, czy upłynął czas symbolTimeout bez sygnału
  if (millis() - lastPressTime > symbolTimeout && morseCode.length() > 0) {
    String letter = morseToLetter(morseCode);
    translatedText += letter;
    morseCode = ""; // reset dla następnej litery

    // Aktualizacja LCD z przetłumaczonym tekstem
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Morse Code:");
    lcd.setCursor(0, 1);
    lcd.print(translatedText);

    Serial.print("Translated: ");
    Serial.println(translatedText);
  }
}

void readButton() {
  if (digitalRead(buttonPin1) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      digitalWrite(buzzerPin, HIGH); // Włącz buzzer
      pressStartTime = millis();
    }
  } else {
    if (buttonPressed) {
      unsigned long pressEndTime = millis();
      unsigned long duration = pressEndTime - pressStartTime;
      digitalWrite(buzzerPin, LOW); // Wyłącz buzzer
      buttonPressed = false;

      Serial.print("Duration: ");
      Serial.println(duration);

      if (duration >= resetThreshold) {
        // Długie naciśnięcie - resetowanie pamięci
        morseCode = "";
        translatedText = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Morse Code:");
        lcd.setCursor(0, 1);
        Serial.println("Memory reset");
      } else if (duration < dotLength) {
        morseCode += ".";
        Serial.println("Dot");
      } else if (duration < dashLength) {
        morseCode += "-";
        Serial.println("Dash");
      } else {
        Serial.println("Too long");
      }

      // Aktualizacja czasu ostatniego sygnału
      lastPressTime = millis();

      // Aktualizacja LCD z bieżącym kodem Morse'a
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Morse Code:");
      lcd.setCursor(0, 1);
      lcd.print(morseCode);
    }
  }
}

void readMicrophone() {
  int micValue = analogRead(micPin);
  Serial.print("Mic Value: ");
  Serial.println(micValue);

  static boolean micSignal = false;
  static unsigned long micSignalStartTime = 0;

  if (micValue > micThreshold) {
    if (!micSignal) {
      micSignal = true;
      micSignalStartTime = millis();
      Serial.println("Mic signal started");
    }
  } else {
    if (micSignal) {
      unsigned long micSignalDuration = millis() - micSignalStartTime;
      micSignal = false;

      Serial.print("Mic Signal Duration: ");
      Serial.println(micSignalDuration);

      if (micSignalDuration < dotLength) {
        morseCode += ".";
        Serial.println("Dot");
      } else if (micSignalDuration < dashLength) {
        morseCode += "-";
        Serial.println("Dash");
      } else {
        Serial.println("Too long");
      }

      // Aktualizacja czasu ostatniego sygnału
      lastPressTime = millis();

      // Aktualizacja LCD z bieżącym kodem Morse'a
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Morse Code:");
      lcd.setCursor(0, 1);
      lcd.print(morseCode);
    }
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
  return "?";
}
