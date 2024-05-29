import serial
import time
import serial.tools.list_ports

# Znajdź dostępne porty
ports = serial.tools.list_ports.comports()
if not ports:
    print("No serial ports found.")
    exit()

# Wybierz pierwszy dostępny port (zakładając, że to Arduino)
port = ports[0].device

# Otwórz połączenie szeregowe
ser = serial.Serial(port, 9600)
time.sleep(2)  # Czas na inicjalizację połączenia

def text_to_morse(text):
    morse_dict = {
        'A': '.-', 'B': '-...', 'C': '-.-.', 'D': '-..', 'E': '.', 'F': '..-.', 'G': '--.', 'H': '....', 'I': '..',
        'J': '.---', 'K': '-.-', 'L': '.-..', 'M': '--', 'N': '-.', 'O': '---', 'P': '.--.', 'Q': '--.-', 'R': '.-.',
        'S': '...', 'T': '-', 'U': '..-', 'V': '...-', 'W': '.--', 'X': '-..-', 'Y': '-.--', 'Z': '--..',
        '1': '.----', '2': '..---', '3': '...--', '4': '....-', '5': '.....', '6': '-....', '7': '--...', '8': '---..',
        '9': '----.', '0': '-----', ' ': '/'
    }

    morse_code = ' '.join(morse_dict[char] for char in text.upper())
    return morse_code

while True:
    text = input("Enter your message: ")
    morse_code = text_to_morse(text)
    print(f"Sending Morse code: {morse_code}")
    ser.write(morse_code.encode() + b'\n')  # Dodaj nową linię, aby zasygnalizować koniec wiadomości
    time.sleep(1)  # Krótkie opóźnienie przed następnym wysyłaniem

ser.close()
