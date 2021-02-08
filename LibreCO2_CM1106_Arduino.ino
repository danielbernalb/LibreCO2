/*
  Reading CO2 from the CM1106, visualize in TM1637 7 segments, buzzer, calibration routine.
  Routine of CM1106 lecture based on agnunez/moco2 repository
  By: Daniel Bernal
  Date: Feb 7, 2021

  Hardware Connections:
  CM1106 with pins to A5 and A4
  TM1637 board connected at pin A2 and A0
  Buzzer connected at pin 8
  Button connected at pin 12

  Leyendo CO2 desde el sensor CM1106 marca Cubic, visualización en el 7 segmentos TM1637, chicharra y rutina de calibración.
  Rutina de lectura del sensor basada en el respositorio agnunez/moco2
  Por: Daniel Bernal
  Fecha: Feb 7, 2021

  Conección de Hardware:
  Sensor CM1106 conectado a A5 y A4
  Board TM1637 conectada a los pines A2 y A0
  Chicharra conectada al pin 8
  Boton o cable conectado a pin 12
*/

const byte PIN_TX = A5;   // define CLK pin (any digital pin)
const byte PIN_RX = A4;   // define DIO pin (any digital pin)
const byte PIN_CLK = A2;   // define CLK pin (any digital pin)
const byte PIN_DIO = A0;   // define DIO pin (any digital pin)
const byte BUTTON = 12;   // define DIO pin (any digital pin)
const byte BUZZER = 8;
unsigned int CO2 = 0;
#define BAUDRATE 9600                                      // Device to CM1106 Serial baudrate (should not be changed)

#include <SoftwareSerial.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
SevenSegmentExtended display(PIN_CLK, PIN_DIO);
SoftwareSerial co2CM(PIN_RX, PIN_TX);


void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Start CM1106 lecture");
  co2CM.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start

  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %

  // Turn off calibration and Sensor connection test
  delay(100);
  Serial.print("CM1106 Stop Autocalibration: ");
  static byte cmd[10] = {0x11, 0x07, 0x10, 0x64, 0x02, 0x07, 0x01, 0x90, 0x64, 0x76}; // Command Close ABC
  static byte response[4] = {0};
  co2CM.write(cmd, 10);
  co2CM.readBytes(response, 4);
  if (response[0] == 0x16 && response[1] == 0x01 && response[2] == 0x10 && response[3] == 0xD9) Serial.println("done");
  else Serial.println(" failed");

  display.clear();
  display.print("good");
  Serial.println("CM1106 read OK");
  delay(5000);


  // Preheat routine: min 20 seconds for CM1106
  display.clear();
  display.print("HEAT");
  Serial.print("Preheat: ");
  delay (3000);
  for (int i = 30; i > -1; i--) { // Preheat from 0 to 30
    display.printNumber(i);
    Serial.println(i);
    delay(1000);
  }
  display.clear();
  display.print("CO2-");
  delay(5000);
}

void loop()
{
  CO2 = co2CM1106();

  if (CO2 > 0) {
    Serial.print("CO2(ppm): ");
    Serial.println(CO2);

    display.clear();
    delay(10);
    display.printNumber(CO2);
  }
  else {
    delay(10);
  }
  // Alarm if CO2 is greater than 1000

  if (CO2 > 1000) {
    Beep();
  }

  // First. put the sensor outside in a close box and wait 5 minutes, this is the reference of 400ppm
  // Open and push the button more than 5 seconds
  // Close the box and the sensor enter to the calibration process
  // At the end the sensor receives the order of calibration to 400ppm

  if (digitalRead(BUTTON) == LOW) {
    delay (2500);
    if (digitalRead(BUTTON) == LOW) {
      delay (2500);
      if (digitalRead(BUTTON) == LOW) {
        Serial.println("Start calibration process: 300 seconds of 400 ppm stable");
        display.clear();
        display.print("CAL-");
        delay(5000);
        for (int i = 300; i > -1; i--) { // loop from 0 to 300
          display.printNumber(i);
          Serial.print(i);
          CO2 = co2CM1106();
          Serial.print(" CO2(ppm): ");
          Serial.println(CO2);
          delay(1000);
        }
        Calibration();
      }
    }
  }

  else {
    Serial.println("Waiting for new data");
    delay(1000);
  }
}

int co2CM1106() {
  static byte cmd[4] = {0x11, 0x01, 0x01, 0xED}; // Commands 0x01 Read ppm, 0x10 open/close ABC, 0x03 Calibrate
  static byte response[8] = {0};                 // response 0x16, 0x05, 0x01, DF1, DF2, DF3, DF4, CRC.  ppm=DF1*256+DF2
  co2CM.write(cmd, 4);
  co2CM.readBytes(response, 8);
  int crc = 0;
  for (int i = 0; i < 7; i++) crc += response[i];
  crc = 256 - crc % 256;
  if (((int) response[0] == 0x16) && ((int)response[7] == crc)) {
    unsigned int responseHigh = (unsigned int) response[3];
    unsigned int responseLow = (unsigned int) response[4];
    return (256 * responseHigh) + responseLow;
  } else {
    while (co2CM.available() > 0)  char t = co2CM.read(); // Clear serial input buffer;
    return 0;
  }
}

void Beep()
{
  tone(BUZZER, 900);
  delay(330);
  noTone(BUZZER);
}

// Calibration routine
void Calibration()
{
  delay(100);
  Serial.print("Resetting forced calibration factor to 400: ");
  static byte cmd[6] = {0x11, 0x03, 0x03, 0x01, 0x90, 0x58}; // Command Calibration of CO2 Concentration to 400ppm
  static byte response[4] = {0};
  co2CM.write(cmd, 6);
  co2CM.readBytes(response, 4);
  if (response[0] == 0x16 && response[1] == 0x01 && response[2] == 0x03 && response[3] == 0xE6) {
    Serial.println("done");
    display.clear();
    display.print("done");
  }
  else {
    Serial.println(" failed");
    display.clear();
    display.print("fail");
  }
  delay(5000);
}
