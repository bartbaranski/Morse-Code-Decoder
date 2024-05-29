#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzerPin = 3;
const int buttonPin1 = 2;
const int buttonPin2 = 4;

boolean buttonPressed = false;
boolean useSerial = false;
String morseCode = "";
String translatedText = "";
unsigned long pressStartTime = 0;
unsigned long lastPressTime = 0;
const unsigned long dotLength = 200;
const unsigned long dashLength = 600;
const unsigned long symbolTimeout = 1000; // czas bezczynności oznaczający koniec symbolu
const unsigned long resetThreshold = 2000; // długie naciśnięcie resetuje pamięć

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
    useSerial = !useSerial;
    delay(300); // debounce
    lcd.clear();
    lcd.setCursor(0, 0);
    if (useSerial) {
      lcd.print("Serial Mode");
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
    morseCode = "";
  }

  if (useSerial) {
    readSerial();
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

void readSerial() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '.') {
      morseCode += ".";
      dot();
    } else if (c == '-') {
      morseCode += "-";
      dash();
    } else if (c == ' ') {
      delay(600); // Przerwa między literami
      // Przetłumacz i zaktualizuj tekst po przerwie między literami
      String letter = morseToLetter(morseCode);
      translatedText += letter;
      morseCode = ""; // reset dla następnej litery

      // Aktualizacja LCD z przetłumaczonym tekstem
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Morse Code:");
      lcd.setCursor(0, 1);
      lcd.print(translatedText);
    } else if (c == '/') {
      delay(1400); // Przerwa między słowami
    }

    // Aktualizacja LCD z bieżącym kodem Morse'a podczas odbierania
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Morse Code:");
    lcd.setCursor(0, 1);
    lcd.print(translatedText);
  }
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
