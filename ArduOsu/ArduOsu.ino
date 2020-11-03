////////////////////////
//                    //
//       Setup        //
//                    //
////////////////////////

// Connect a 1000μF cap to 5v and ground
// Connect a 100μF cap to a 10k resistor and ground, that resistor connects to A3
// This is for the power off save function


// Connect up 4 switches, 1 pin of the switch goes to ground,
// and the other pin goes to the following arduino pin

// Switch 1 -> pin 2
// Switch 2 -> pin 4
// Switch 3 -> pin 7
// Switch 4 -> pin 8

////////////////////////
//                    //
//  Special features  //
//                    //
////////////////////////

// To reset the click counter inside the EEPROM, hold the 1st and 4th button and then power up the keypad
// This will reset the counter if done correctly

// If you don't want to use the serial monitor, press the 1st button when it is flashing.
// This will skip the serial setup

#include <EEPROM.h>
#include "Keyboard.h"

int timer1_counter;

long button1_presses;
long button2_presses;
long button3_presses;
long button4_presses;

bool button1_clickrunonce;
bool button2_clickrunonce;
bool button3_clickrunonce;
bool button4_clickrunonce;

int brightness_LED1;
int brightness_LED2;
int brightness_LED3;
int brightness_LED4;

int pin_LED1 = 3;
int pin_LED2 = 5;
int pin_LED3 = 6;
int pin_LED4 = 9;

int pin_button1 = 2;
int pin_button2 = 4;
int pin_button3 = 7;
int pin_button4 = 8;

float joulePerTap = 0.0096064;
float joulPerCalorie = 4184;
float jouleCalculate;
float caloriesCalculate;
long totalButtonPresses = 0;

int blinkCounter = 0;

bool shutdownTriggered = false;

bool updateCounter = false; //we only want to save at power off if there actually has been atleast 1 click

String inData; //Serial command buffer

bool applicationMode = false;

void setup() {

  setupPins();

  button1_presses = EEPROMReadlong(1);
  button2_presses = EEPROMReadlong(5);
  button3_presses = EEPROMReadlong(9);
  button4_presses = EEPROMReadlong(13);

  if (digitalRead(pin_button1) == LOW && digitalRead(pin_button4) == LOW) {
    //Reset the EEPROM
    delay(1000);
    button1_presses = 0;
    button2_presses = 0;
    button3_presses = 0;
    button4_presses = 0;
    EEPROMWritelong(1, button1_presses);
    digitalWrite(pin_LED1, HIGH);
    delay(100);
    EEPROMWritelong(5, button2_presses);
    digitalWrite(pin_LED2, HIGH);
    delay(100);
    EEPROMWritelong(9, button3_presses);
    digitalWrite(pin_LED3, HIGH);
    delay(100);
    EEPROMWritelong(13, button4_presses);
    digitalWrite(pin_LED4, HIGH);
    delay(1000);
    digitalWrite(pin_LED1, LOW);
    digitalWrite(pin_LED2, LOW);
    digitalWrite(pin_LED3, LOW);
    digitalWrite(pin_LED4, LOW);
  }

  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    if (blinkCounter == 50) {
      digitalWrite(pin_LED1, HIGH);
    } else if (blinkCounter == 100) {
      digitalWrite(pin_LED1, LOW);
      blinkCounter = 0;
    }

    if (digitalRead(pin_button1) == LOW) {
      break;
    }

    blinkCounter += 1;
  }

  delay(100);
  checkSerial(true); //check if we received a message to switch to application mode

  if (!applicationMode) {
    printLogo();
    Serial.println("Arduosu - Arduino powered keypad for Osu!");
    Serial.print("Compile date: ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);
    Serial.println("Original build date: Jan 19 2019 15:47:15");
    Serial.println();
    Serial.print("Safety startup delay");

    for (int i = 0; i <= 40; i++) {
      Serial.print(".");
      analogWrite(pin_LED1, i * 6);
      analogWrite(pin_LED2, i * 6);
      analogWrite(pin_LED3, i * 6);
      analogWrite(pin_LED4, i * 6);
      delay(25);
    }
    Serial.println("OK");
    Serial.print("Initializing keyboard...");
    Keyboard.begin();
    Serial.println("OK");
    Serial.print("Setting up interrupts...");
    setupInterrupt();
    Serial.println("OK");
    Serial.println("Ready!");
    Serial.println();
    printLine();
    Serial.println();
    Serial.println("type help to get a list of available commands");
    Serial.println();
    printLine();
  } else {
    delay(1000);
    Keyboard.begin();
    setupInterrupt();
    Serial.println("init_ok");
  }
}

void loop() {
  if (digitalRead(pin_button1) == LOW) {
    Keyboard.press('z');
    updateCounter = true;
    if (!applicationMode) {
      brightness_LED1 = 255;
    }
    if (button1_clickrunonce) {
      button1_clickrunonce = false;
      button1_presses += 1;
    }
  } else {
    Keyboard.release('z');
    button1_clickrunonce = true;
  }

  if (digitalRead(pin_button2) == LOW) {
    Keyboard.press('x');
    updateCounter = true;
    if (!applicationMode) {
      brightness_LED2 = 255;
    }
    if (button2_clickrunonce) {
      button2_clickrunonce = false;
      button2_presses += 1;
    }
  } else {
    Keyboard.release('x');
    button2_clickrunonce = true;
  }

  if (digitalRead(pin_button3) == LOW) {
    Keyboard.press('c');
    updateCounter = true;
    if (!applicationMode) {
      brightness_LED3 = 255;
    }
    if (button3_clickrunonce) {
      button3_clickrunonce = false;
      button3_presses += 1;
    }
  } else {
    Keyboard.release('c');
    button3_clickrunonce = true;
  }

  if (digitalRead(pin_button4) == LOW) {
    Keyboard.press('v');
    updateCounter = true;
    if (!applicationMode) {
      brightness_LED4 = 255;
    }
    if (button4_clickrunonce) {
      button4_clickrunonce = false;
      button4_presses += 1;
    }
  } else {
    Keyboard.release('v');
    button4_clickrunonce = true;
  }

  updateBacklightBrightness();
  checkSerial(false);
}

void setupPins() {
  pinMode(pin_button1, INPUT_PULLUP);
  pinMode(pin_button2, INPUT_PULLUP);
  pinMode(pin_button3, INPUT_PULLUP);
  pinMode(pin_button4, INPUT_PULLUP);

  pinMode(pin_LED1, OUTPUT);
  pinMode(pin_LED2, OUTPUT);
  pinMode(pin_LED3, OUTPUT);
  pinMode(pin_LED4, OUTPUT);
}

void updateBacklightBrightness() {
  analogWrite(pin_LED1, brightness_LED1);
  analogWrite(pin_LED2, brightness_LED2);
  analogWrite(pin_LED3, brightness_LED3);

  //Timer1 is already in use, so we can't use analogWrite on pin 9
  //We need to blink the LED manually really fast
  if (brightness_LED4 > 0) {
    digitalWrite(pin_LED4, HIGH);
    delayMicroseconds(map(brightness_LED4, 0, 255, 0, 5000));
    digitalWrite(pin_LED4, LOW);
  } else {
    digitalWrite(pin_LED4, LOW);
  }
}

void setupInterrupt() {
  // initialize timer1
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz

  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER1_OVF_vect)        // interrupt service routine
{
  TCNT1 = timer1_counter;   // preload timer

  if (shutdownTriggered == false) {
    CheckPower(); // Autosave when the power is lost
  }

  if (!applicationMode) {
    if (brightness_LED1 > 0) {
      brightness_LED1 -= 5;
    }

    if (brightness_LED2 > 0) {
      brightness_LED2 -= 5;
    }

    if (brightness_LED3 > 0) {
      brightness_LED3 -= 5;
    }

    if (brightness_LED4 > 0) {
      brightness_LED4 -= 5;
    }
  }

}

void CheckPower() {
  if (analogRead(A3) < 928) {
    // Charge the cap up a bit
    pinMode(A3, OUTPUT);
    digitalWrite(A3, HIGH);
    return;
  }

  pinMode(A3, INPUT);

  if (analogRead(A3) > 1010) {
    // We lost power, save everything if needed

    digitalWrite(pin_LED1, LOW);
    digitalWrite(pin_LED2, LOW);
    digitalWrite(pin_LED3, LOW);
    digitalWrite(pin_LED4, LOW);

    if (updateCounter) { //Check if we actually have to save
      EEPROMWritelong(1, button1_presses);
      EEPROMWritelong(5, button2_presses);
      EEPROMWritelong(9, button3_presses);
      EEPROMWritelong(13, button4_presses);
    }

    shutdownTriggered = true;
    brightness_LED1 = 0;
    brightness_LED2 = 0;
    brightness_LED3 = 0;
    brightness_LED4 = 0;
  }
}

void EEPROMWritelong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void printLogo() {
  Serial.println(" _______          __                           __ ");
  Serial.println("|   _   |.----.--|  |.--.--.-----.-----.--.--.|  |");
  Serial.println("|       ||   _|  _  ||  |  |  _  |__ --|  |  ||__|");
  Serial.println("|___|___||__| |_____||_____|_____|_____|_____||__|");
  Serial.println();
}

void printButtonPresses() {
  Serial.print("Button 1 presses: ");
  Serial.println(button1_presses);
  Serial.print("Button 2 presses: ");
  Serial.println(button2_presses);
  Serial.print("Button 3 presses: ");
  Serial.println(button3_presses);
  Serial.print("Button 4 presses: ");
  Serial.println(button4_presses);
  Serial.println();

  totalButtonPresses = button1_presses + button2_presses + button3_presses + button4_presses;
  jouleCalculate = joulePerTap * totalButtonPresses;
  caloriesCalculate = jouleCalculate / joulPerCalorie;
  Serial.print("Total button presses: ");
  Serial.println(totalButtonPresses);
  Serial.print("Wasted ");
  Serial.print(joulePerTap * totalButtonPresses);
  Serial.print("J of energy on clicking circles (or ");
  Serial.print(caloriesCalculate);
  Serial.println(" calories)");
}

void checkSerial(bool startupCheck) {
  while (Serial.available() > 0)
  {
    char recieved = Serial.read();

    // Process message when new line character is recieved
    if (recieved == '\n')
    {
      if (startupCheck) {
        if (inData == "application_mode") {
          applicationMode = true;
          Serial.println("application_mode OK");
        }
      } else {
        if (applicationMode) {
          if (inData.startsWith("set_leds")) {
            //char *str;
            String str;
            char sz[24];
            inData.toCharArray(sz, 24);
            char *p = sz;
            byte counter = 0;
            while ((str = strtok_r(p, ",", &p)) != NULL) {
              switch (counter) {
                case 1:
                  brightness_LED1 = str.toInt();
                  break;
                case 2:
                  brightness_LED2 = str.toInt();
                  break;
                case 3:
                  brightness_LED3 = str.toInt();
                  break;
                case 4:
                  brightness_LED4 = str.toInt();
                  break;
              }
              counter += 1;
            }
          }
        } else {
          if (inData == "help") {
            Serial.println("Available commands");
            Serial.println();
            Serial.println("help - show all the available commands");
            Serial.println("update - displays the current amount of button presses");
            Serial.println("save - saves the current amount of button presses to the EEPROM");
            Serial.println();
          } else if (inData == "update") {
            printButtonPresses();
            Serial.println();
          } else if (inData == "save") {
            Serial.print("Saving to EEPROM...");
            EEPROMWritelong(1, button1_presses);
            EEPROMWritelong(5, button2_presses);
            EEPROMWritelong(9, button3_presses);
            EEPROMWritelong(13, button4_presses);
            updateCounter = false;
            Serial.println("OK");
            Serial.println();
          } else {
            Serial.print(inData);
            Serial.println(" is not a valid command!");
            Serial.println("type help to get a list of available commands");
            Serial.println();
          }

          printLine();
        }
      }

      inData = ""; // Clear recieved buffer
    } else {
      inData += recieved;
    }
  }
}

void printLine() {
  for (int i = 0; i <= 80; i++) {
    Serial.print("-");
  }

  Serial.println();
}
