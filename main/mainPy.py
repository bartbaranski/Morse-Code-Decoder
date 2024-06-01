import tkinter as tk
from tkinter import messagebox
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

def send_message():
    text = message_entry.get()
    morse_code = text_to_morse(text)
    print(f"Sending Morse code: {morse_code}")
    ser.write(morse_code.encode() + b'\n')  # Dodaj nową linię, aby zasygnalizować koniec wiadomości
    message_entry.delete(0, tk.END)

def limit_input(*args):
    value = message_var.get()
    if len(value) > 16:
        message_var.set(value[:16])

# Tworzenie głównego okna aplikacji
root = tk.Tk()
root.title("Morse Code Sender")

# Etykieta dla pola wpisywania wiadomości
message_label = tk.Label(root, text="Enter your message (max 16 chars):")
message_label.pack(pady=5)

# Zmienna powiązana z polem wpisywania wiadomości
message_var = tk.StringVar()
message_var.trace("w", limit_input)

# Pole wpisywania wiadomości
message_entry = tk.Entry(root, textvariable=message_var, width=20)
message_entry.pack(pady=5)

# Przycisk "Wyślij"
send_button = tk.Button(root, text="Send", command=send_message)
send_button.pack(pady=20)

# Uruchomienie pętli głównej aplikacji
root.mainloop()

# Zamknięcie portu szeregowego przy zamknięciu aplikacji
if ser and ser.is_open:
    ser.close()
