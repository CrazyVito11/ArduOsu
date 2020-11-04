# Arduosu
Arduino powered keypad for Osu! and other rhythm games

## Features
- Individual key backlight control
- Button presses counter

## Required hardware
- Arduino Pro Micro (or any other ATmega32U4 based board)
- 4x (mechanical) switches with keycap
- 4x LEDs 3mm (optional, requires mechanical switch)
- 5x 10k resitor (or just 1 if you don't want backlight)
- 1000μF capacitor that can handle at least 5 volts (I used a 63V 1000μF)
- 100μF capacitor that can handle at least 5 volts (I used a 25V 100μF)

## Pinout
|Arduino|Component                                                       |
|-------|----------------------------------------------------------------|
|2      | Button #1 (other pin to GRN)                                   |
|3      | LED #1 (through a 10K resistor, other pin to GRN)              |
|4      | Button #2 (other pin to GRN)                                   |
|5      | LED #2 (through a 10K resistor, other pin to GRN)              |
|6      | LED #3 (through a 10K resistor, other pin to GRN)              |
|7      | Button #3 (other pin to GRN)                                   |
|8      | Button #4 (other pin to GRN)                                   |
|9      | LED #4 (through a 10K resistor, other pin to GRN)              |
|A3     | 100μF capacitor + pin through a 10K resistor, other pin to GRN |
|5V     | 1000μF capacitor + pin, other pin to GRN                       |

## FAQ
### The first button is blinking!
That means the keypad is waiting for a serial connection to begin.

If you don't want to use the serial connection, then press the blinking button.

### How do I reset the button press counter?
First, make sure that the keypad is shut down.

Then, hold button 1 and 4, and then turn on the keypad.


A small animation should play to indicate that the reset has been triggered.

The keypad has now been reset.
