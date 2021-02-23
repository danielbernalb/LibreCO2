/*
  Reading CO2 from the SCD30, visualize in TM1637 7 segments, buzzer, calibration routine.
  Routine of SCD30 lecture based on Modbus protocol
  By: Daniel Bernal
  Date: Feb 7, 2021
  
  Hardware Connections:
  SCD30 with pins to 7 and 6
  TM1637 board connected at pin 9 and 8
  Buzzer connected at pin 11
  Button connected at pin 4
  
  Leyendo CO2 desde el sensor SCD30 marca Cubic, visualización en el 7 segmentos TM1637, chicharra y rutina de calibración.
  Rutina de lectura del sensor basada en el respositorio agnunez/moco2
  Por: Daniel Bernal
  Fecha: Feb 7, 2021
  
  Conección de Hardware:
  Sensor CM1106 conectado a 7 y 6
  Board TM1637 conectada a los pines 9 y 8
  Chicharra conectada al pin 11
  Boton o cable conectado a pin 4
*/

const byte PIN_TX = 6;   // define CLK pin (any digital pin)
const byte PIN_RX = 7;   // define DIO pin (any digital pin)
const byte PIN_CLK = 9;   // define CLK pin (any digital pin)
const byte PIN_DIO = 8;   // define DIO pin (any digital pin)
const byte BUTTON = 2;   // define DIO pin (any digital pin)
const byte BUZZER = 11;
const int MB_PKT_8 = 8;  //MODBUS Packet Sizes
const int MB_PKT_17 = 17;


unsigned int CO2 = 0;
unsigned int ConnRetry = 0;

union BYTE_FLOAT_CO2 {
  byte uByte[4];
  float uCO2;
} u;

float co2Concentration;

static byte response[MB_PKT_8] = {0};
unsigned char buffer[4];

#define BAUDRATE 19200      // Device to SCD30 Serial baudrate (should not be changed)

#include <SoftwareSerial.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include <avr/wdt.h>
SevenSegmentExtended display(PIN_CLK, PIN_DIO);
SoftwareSerial co2SCD(PIN_RX, PIN_TX);


void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  Serial.begin(115200);
  Serial.println("Start SCD30 Modbus lecture");
  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %
  co2SCD.begin(BAUDRATE);      // (Uno example) device to SCD30 serial start
  delay(1000);

  //0x61 0x06 0x00 0x36 0x00 0x00 0x60 0x64
  //Trigger continuous measurement with no ambient pressure compensation

  static byte cmd[MB_PKT_8] = {0x61, 0x06, 0x00, 0x36, 0x00, 0x00, 0x60, 0x64};
  co2SCD.write(cmd, MB_PKT_8);
  co2SCD.readBytes(response, MB_PKT_8);

  Serial.println("Start continious measurement");
  array_print_hex(response,MB_PKT_8);

  if (array_cmp(cmd,response,MB_PKT_8)) 
  {
    Serial.println("done");
    display.clear();
    delay(15);
    display.print("done");
  }
  else {
    Serial.println(" failed");
    display.clear();
    display.print("fail");
  }

  delay(1000);

  // 0x61 0x06 0x00 0x25 0x00 0x02 0x10 0x60
  //Set measurement interval 2 seconds

  static byte cmd2[MB_PKT_8] = {0x61, 0x06, 0x00, 0x25, 0x00, 0x02, 0x10, 0x60};
  //response = {0};
  co2SCD.write(cmd2, MB_PKT_8);
  co2SCD.readBytes(response, MB_PKT_8);

  Serial.println("Set measurement interval 2 seconds ");
  array_print_hex(response,MB_PKT_8);

  if (array_cmp(cmd2,response,MB_PKT_8)) 
  {
    Serial.println("done");
    display.clear();
    delay(15);
    display.print("done");
  }
  else {
    Serial.println(" failed");
    display.clear();
    display.print("fail");
  }

  delay(1000);

  //Deactivate Automatic Self-Calibration
  //  0x61 0x06 0x00 0x3A 0x00 0x00 0xA0 0x67

  static byte cmd3[MB_PKT_8] = {0x61, 0x06, 0x00, 0x3A, 0x00, 0x00, 0xA0, 0x67};
  //response = {0};
  co2SCD.write(cmd3, MB_PKT_8);
  co2SCD.readBytes(response, MB_PKT_8);

  Serial.println("Deactivate Automatic Self-Calibration ");
  array_print_hex(response,MB_PKT_8);
  
  //Auto calibration set to
  //0x61 0x03 0x00 0x3A 0x00 0x01 0xAD 0xA7

  static byte cmd4[MB_PKT_8] = {0x61, 0x03, 0x00, 0x3A, 0x00, 0x01, 0xAD, 0xA7};
  //response = {0};
  co2SCD.write(cmd4, MB_PKT_8);
  co2SCD.readBytes(response, 7);

  Serial.println("Autocalibration");
  array_print_hex(response,7);
 
  Serial.print("Autocalibration set to: ");
  if (response[4] == 00)
  { Serial.print(" OFF");
    display.clear();
    delay(15);
    display.print("done");
  }
  else {
    Serial.print(" ON");
    display.clear();
    display.print("fail");
  }
  Serial.println();
  delay(1000);

  while (co2SCD30() == 0 && (ConnRetry < 5))
  {
    Serial.println("Air sensor not detected. Please check wiring... Try# " + String(ConnRetry));
    display.clear();
    delay(20);
    display.print("bad");
    delay(2500);
    ConnRetry++;
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println("Air sensor detected SPS30 Modbus");
  display.clear();
  display.print("good");
  Serial.println("SCD30 read OK");
  delay(5000);


  // Preheat routine: min 30 seconds for SCD30
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
  CO2 = co2SCD30();

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
          CO2 = co2SCD30();
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
    delay(2000);
  }
}

int co2SCD30() {

  static byte cmd2[MB_PKT_8] = {0x61, 0x03, 0x00, 0x28, 0x00, 0x06, 0x4C, 0x60};
  static byte response2[MB_PKT_17] = {0};
  co2SCD.write(cmd2, MB_PKT_8);
  co2SCD.readBytes(response2, MB_PKT_17);

  Serial.println("Read Sensor");  
  array_print_hex(response2,MB_PKT_17);
  
  u.uByte[0] = response2[6];
  u.uByte[1] = response2[5];
  u.uByte[2] = response2[4];
  u.uByte[3] = response2[3];

//  Serial.print("Value CO2: ");
//  Serial.println(u.uCO2, 3); //3 decimales
  return (u.uCO2);
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

  //Calibration set to 400ppm
  //0x61 0x06 0x00 0x39 0x01 0x90 0x51 0xB9

  static byte cmd7[MB_PKT_8] = {0x61, 0x06, 0x00, 0x39, 0x01, 0x90, 0x51, 0xB9};
  co2SCD.write(cmd7, MB_PKT_8);
  co2SCD.readBytes(response, MB_PKT_8);

  Serial.print("Resetting forced calibration factor to 400: ");
  array_print_hex(response,MB_PKT_8);
 
  if (array_cmp(cmd7,response,MB_PKT_8)) 
  {
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

void softwareReset( uint8_t prescaller) {
  wdt_enable( prescaller);
  while (1) {}
}

boolean array_cmp(uint8_t *a, uint8_t *b, uint8_t len_array_cmp)
{
     for (int n=0;n<len_array_cmp;n++) if (a[n]!=b[n]) return false;
     return true;
} 

void array_print_hex( uint8_t array_to_print[], int array_to_print_size)
{
  for  (int n=0;n<array_to_print_size;n++) 
        {
        Serial.print(array_to_print[n], HEX);
        Serial.print(" ");
        }
   Serial.println(" ");
}
