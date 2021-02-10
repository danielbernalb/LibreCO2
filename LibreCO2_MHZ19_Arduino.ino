/*
  Reading CO2 from the MH-Z14 or MH-Z19, visualize in TM1637 7 segments, buzzer, calibration routine.
  Routine of MH-Z14 or MH-Z19 lecture based on Wifwaf library
  By: Daniel Bernal
  Date: Feb 7, 2021

  Hardware Connections:
  MHZ14 or MHZ19 connected at pin A5 and A4
  TM1637 board connected at pin A2 and A0
  Buzzer connected at pin 8
  Button connected at pin 12

  Leyendo CO2 desde el sensor MH-Z14 or MH-Z19 marca Winsen, visualización en el 7 segmentos TM1637, chicharra y rutina de calibración.
  Rutina de lectura del sensor basada en la libreria Wifwaf
  Por: Daniel Bernal
  Fecha: Feb 7, 2021

  Conección de Hardware:
  Sensor MH-Z14 or MH-Z19 conectado a A5 y A4
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
int Temp = 0;
#define BAUDRATE 9600    // Device to Mhz14 Mhz19 Serial baudrate (should not be changed)

#include <SoftwareSerial.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include "MHZ19.h"
SevenSegmentExtended display(PIN_CLK, PIN_DIO);
SoftwareSerial mySerial(PIN_RX, PIN_TX);
MHZ19 co2MHZ;

void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Start MHZ14 or MHZ19 lecture");
  mySerial.begin(BAUDRATE);   // (Uno example) device to MH-Z19 serial start
  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %
  co2MHZ.begin(mySerial);     // *Serial(Stream) refence must be passed to library begin().
  delay(1000);

  // Connection test

  if (co2MHZ.getCO2() == 0) {
    Serial.println("Air sensor not detected. Please check wiring...");
    display.print("bad");
    delay(5000);
    if (co2MHZ.getCO2() == 0) {
      Serial.println("Air sensor not detected. Please check wiring...");
      display.print("bad");
      delay(5000);
      if (co2MHZ.getCO2() == 0) {
        Serial.println("Air sensor not detected. Please check your wiring");
        Serial.println("Freeze.....RESET the Arduino");
        display.print("bad-");
        while (1);
      }
    }
  }

  // Turn off calibration and Sensor connection test
  delay(100);
  Serial.print("MHZ19 Stop Autocalibration: ");
  co2MHZ.autoCalibration(false);                               // Turn OFF calibration
  Serial.print("MHZ19 Autocalibration: ");
  co2MHZ.getABC() ? Serial.println(" failed") :  Serial.println(" done");  // now print it's status
  delay(100);

  display.clear();
  display.print("good");
  Serial.println("MHZ19 read OK");
  delay(5000);


  // Preheat routine: 3 minutes for MHZ19
  display.clear();
  display.print("HEAT");
  Serial.print("Preheat: ");
  delay (3000);
  for (int i = 180; i > -1; i--) { // Preheat from 0 to 180
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
  CO2 = co2MHZ.getCO2();                             // Request CO2 (as ppm)

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

  Temp = co2MHZ.getTemperature();                     // Request Temperature (as Celsius)
  Serial.print("Temp(°C): ");
  Serial.println(Temp);

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
        Serial.println("Start calibration process: 1200 seconds of 400 ppm stable");
        display.clear();
        display.print("CAL-");
        delay(5000);
        for (int i = 1200; i > -1; i--) { // loop from 0 to 20 minutes
          display.printNumber(i);
          Serial.print(i);
          CO2 = co2MHZ.getCO2();
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
  co2MHZ.calibrate();    // Take a reading which be used as the zero point for 400 ppm
  Serial.println("done");
  display.clear();
  display.print("done");
  delay(5000);
}