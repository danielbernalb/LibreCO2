/*
  Reading CO2, humidity and temperature from the SCD30, visualize in TM1637 7 segments, buzzer, calibration routine.
  Modbus serial mode
  By: Daniel Bernal
  Date: Feb 2, 2021

  Hardware Connections:
  SCD30 VCC to 3.3V
  SEL pin to 3.3V
  SCD30 with pins connected to 6 and 7 pin
  TM1637 board connected at pin 8 and 9
  Buzzer connected at pin 11
  Button connected at pin 2

  Leyendo CO2, humedad y temeratura desde el SCD30 marca Sensirion, visualización en el 7 segmentos TM1637, chicharra y rutina de calibración.
  Protoloco Modbus serial
  Por: Daniel Bernal
  Fecha: Feb 2, 2021

  Conexión de Hardware:
  SCD30 alimentacion a 3.3V
  Pin SEL a 3.3V
  Sensor SCD30 con pines conectados a 6 y 7
  Board TM1637 conectada a los pines 8 y 9
  Chicharra conectada al pin 11
  Boton o cable conectado a pin 2
*/

const byte PIN_TX = 6;   // define CLK pin (any digital pin)
const byte PIN_RX = 7;   // define DIO pin (any digital pin)
const byte PIN_CLK = 9;   // define CLK pin (any digital pin)
const byte PIN_DIO = 8;   // define DIO pin (any digital pin)
const byte BUTTON = 2;   // define DIO pin (any digital pin)
const byte BUZZER = 11;
unsigned int CO2 = 0;

float co2Concentration;
unsigned long tempU32;

//static byte cmd[8] = {0};
static byte response[8] = {0};

unsigned char buffer[4];

#define BAUDRATE 19200      // Device to CM1106 Serial baudrate (should not be changed)

#include <SoftwareSerial.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
SevenSegmentExtended display(PIN_CLK, PIN_DIO);
SoftwareSerial co2CM(PIN_RX, PIN_TX);


void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  Serial.begin(115200);
  Serial.println("Start SCD30 Modbus lecture");
  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %
  co2CM.begin(BAUDRATE);      // (Uno example) device to CM1106 serial start
  delay(1000);

  // Connection test

  //0x61 0x06 0x00 0x36 0x00 0x00 0x60 0x64
  //Trigger continuous measurement with optional ambient pressure compensation

  static byte cmd[8] = {0x61, 0x06, 0x00, 0x36, 0x00, 0x00, 0x60, 0x64};
  //response = {0};
  co2CM.write(cmd, 8);
  co2CM.readBytes(response, 8);

  Serial.println("Start continious measurement");
  Serial.print(response[0], HEX);
  Serial.print(" ");
  Serial.print(response[1], HEX);
  Serial.print(" ");
  Serial.print(response[2], HEX);
  Serial.print(" ");
  Serial.print(response[3], HEX);
  Serial.print(" ");
  Serial.print(response[4], HEX);
  Serial.print(" ");
  Serial.print(response[5], HEX);
  Serial.print(" ");
  Serial.print(response[6], HEX);
  Serial.print(" ");
  Serial.print(response[7], HEX);
  Serial.println();

  delay(1000);

  // 0x61 0x06 0x00 0x25 0x00 0x02 0x10 0x60

  //Set measurement interval 2 seconds

  static byte cmd2[8] = {0x61, 0x06, 0x00, 0x25, 0x00, 0x02, 0x10, 0x60};
  //response = {0};
  co2CM.write(cmd2, 8);
  co2CM.readBytes(response, 8);

  Serial.println("Set measurement interval 2 seconds ");
  Serial.print(response[0], HEX);
  Serial.print(" ");
  Serial.print(response[1], HEX);
  Serial.print(" ");
  Serial.print(response[2], HEX);
  Serial.print(" ");
  Serial.print(response[3], HEX);
  Serial.print(" ");
  Serial.print(response[4], HEX);
  Serial.print(" ");
  Serial.print(response[5], HEX);
  Serial.print(" ");
  Serial.print(response[6], HEX);
  Serial.print(" ");
  Serial.print(response[7], HEX);
  Serial.println();

  delay(1000);


  //Deactivate Automatic Self-Calibration
  //  0x61 0x06 0x00 0x3A 0x00 0x00 0xA0 0x67

  static byte cmd3[8] = {0x61, 0x06, 0x00, 0x3A, 0x00, 0x00, 0xA0, 0x67};
  //response = {0};
  co2CM.write(cmd3, 8);
  co2CM.readBytes(response, 8);

  Serial.println("Deactivate Automatic Self-Calibration ");
  Serial.print(response[0], HEX);
  Serial.print(" ");
  Serial.print(response[1], HEX);
  Serial.print(" ");
  Serial.print(response[2], HEX);
  Serial.print(" ");
  Serial.print(response[3], HEX);
  Serial.print(" ");
  Serial.print(response[4], HEX);
  Serial.print(" ");
  Serial.print(response[5], HEX);
  Serial.print(" ");
  Serial.print(response[6], HEX);
  Serial.print(" ");
  Serial.print(response[7], HEX);
  Serial.println();

  delay(1000);

  //Auto calibration set to
  //0x61 0x03 0x00 0x3A 0x00 0x01 0xAD 0xA7

  static byte cmd4[8] = {0x61, 0x03, 0x00, 0x3A, 0x00, 0x01, 0xAD, 0xA7};
  //response = {0};
  co2CM.write(cmd4, 8);
  co2CM.readBytes(response, 7);

  Serial.println("Autocalibration");
  Serial.print(response[0], HEX);
  Serial.print(" ");
  Serial.print(response[1], HEX);
  Serial.print(" ");
  Serial.print(response[2], HEX);
  Serial.print(" ");
  Serial.print(response[3], HEX);
  Serial.print(" ");
  Serial.print(response[4], HEX);
  Serial.print(" ");
  Serial.print(response[5], HEX);
  Serial.print(" ");
  Serial.print(response[6], HEX);
  Serial.println();
//  Serial.print(response[7], HEX);
//  Serial.println();
  
  Serial.print("Autocalibration set to: ");
  if (response[4] == 00) Serial.print(" OFF");
  else Serial.print(" ON");
  Serial.println();
  delay(1000);


  /*
    //0x61 0x03 0x00 0x20 0x00 0x01 0x8C 0x60
    //Read firmware version

    static byte cmd3[8] = {0x61, 0x03, 0x00, 0x20, 0x00, 0x01, 0x8C, 0x60};
    static byte response3[7] = {0};
    co2CM.write(cmd3, 8);
    co2CM.readBytes(response3, 7);

    Serial.println("Read Version");
    Serial.println(response3[0], HEX);
    Serial.println(response3[1], HEX);
    Serial.println(response3[2], HEX);
    Serial.println(response3[3], HEX);
    Serial.println(response3[4], HEX);
    Serial.println(response3[5], HEX);
    Serial.println(response3[6], HEX);
  */

  while (1) {
    delay(2000);

    static byte cmd2[8] = {0x61, 0x03, 0x00, 0x28, 0x00, 0x06, 0x4C, 0x60};
    static byte response2[17] = {0};
    co2CM.write(cmd2, 8);
    co2CM.readBytes(response2, 17);

    Serial.println("Read Sensor");
    Serial.print(response2[0], HEX);
    Serial.print(" ");
    Serial.print(response2[1], HEX);
    Serial.print(" ");
    Serial.print(response2[2], HEX);
    Serial.print(" ");
    Serial.print(response2[3], HEX); //CO2 MMSB
    Serial.print(" ");
    Serial.print(response2[4], HEX); //CO2 MLSB
    Serial.print(" ");
    Serial.print(response2[5], HEX); //CO2 LMSB
    Serial.print(" ");
    Serial.print(response2[6], HEX); //CO2 LLSB
    Serial.print(" ");
    Serial.print(response2[7], HEX);
    Serial.print(" ");
    Serial.print(response2[8], HEX);
    Serial.print(" ");
    Serial.print(response2[9], HEX);
    Serial.print(" ");
    Serial.print(response2[10], HEX);
    Serial.print(" ");
    Serial.print(response2[11], HEX);
    Serial.print(" ");
    Serial.print(response2[12], HEX);
    Serial.print(" ");
    Serial.print(response2[13], HEX);
    Serial.print(" ");
    Serial.print(response2[14], HEX);
    Serial.print(" ");
    Serial.print(response2[15], HEX);
    Serial.print(" ");
    Serial.println(response2[16], HEX);

    tempU32 = 0;
    tempU32 = (((response2[3]) << 24) |
               ((response2[4]) << 16) |
               ((response2[5]) << 8) |
               (response2[6]));

    Serial.println("Version1");
    Serial.println(tempU32);
    co2Concentration = (*(float*)&tempU32);

    Serial.println(co2Concentration);


    tempU32 = 0;
    buffer[0] = 0x43; // MMSB CO2
    buffer[1] = 0xDB; // MLSB CO2
    buffer[2] = 0x8C; // LMSB CO2
    buffer[3] = 0x2E; // LLSB CO2
    // cast 4 bytes to one unsigned 32 bit integer
    tempU32 = (((buffer[0]) << 24) |
               ((buffer[1]) << 16) |
               ((buffer[2]) << 8) |
               (buffer[3]));

    // cast unsigned 32 bit integer to 32 bit float
    co2Concentration = (*(float*)&tempU32); // co2Concentration = 439.09f

    Serial.println("Version2");
    Serial.println(tempU32);
    Serial.println(co2Concentration);



  }

}


  if (co2CM1106() == 0) {
    Serial.println("Air sensor not detected. Please check wiring...");
    display.print("bad");
    delay(5000);
    if (co2CM1106() == 0) {
      Serial.println("Air sensor not detected. Please check wiring...");
      display.print("bad");
      delay(5000);
      if (co2CM1106() == 0) {
        Serial.println("Air sensor not detected. Please check wiring");
        Serial.println("Freezed.....RESET the Arduino");
        display.print("bad-");
        while (1);
      }
    }
  }

  // Turn off calibration and Sensor connection test

  display.clear();
  display.print("good");
  Serial.println("CM1106 read OK");
  delay(5000);


  // Preheat routine: min 30 seconds for CM1106
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