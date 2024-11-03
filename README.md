<h1 align="center">Morse Code Converter Project</h1>

<p align="center">
  <b>A Python application for converting text to Morse code and sending it to an Arduino over a serial port.</b>
</p>

<h2>Overview</h2>
<p>This project provides a Python GUI that allows users to enter text, convert it to Morse code, and transmit the Morse code to an Arduino device via a serial connection. The Arduino then interprets and displays the Morse code on an LCD screen and generates audio or visual signals.</p>

<h2>Features</h2>
<ul>
  <li>Convert any text input to Morse code.</li>
  <li>Send Morse code to an Arduino using a serial connection.</li>
  <li>Display Morse code visually on an LCD and via LED signals on the Arduino.</li>
</ul>

<h2>Requirements</h2>
<ul>
  <li>Python 3.x</li>
  <li>PySerial (for serial communication)</li>
  <li>PyInstaller (for converting Python scripts to executables, if needed)</li>
  <li>Arduino with LCD and LED setup</li>
</ul>

<h2>Arduino Setup</h2>
<p>This project includes Arduino code written in C++. The code enables the Arduino to receive Morse code signals over a serial connection, interpret them, and display the decoded characters on an LCD screen.</p>
<p>Ensure your Arduino setup includes:</p>
<ul>
  <li>An LCD screen (for displaying Morse code text)</li>
  <li>LED and buzzer connections (for visual and auditory Morse code signaling)</li>
  <li>Correct wiring and configuration to support the code</li>
</ul>
