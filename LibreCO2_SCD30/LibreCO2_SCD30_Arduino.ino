/*
  Reading CO2, humidity and temperature from the SCD30, visualize in TM1637 7 segments, buzzer, calibration routine.
  Based on SparkFun_SCD30_Arduino_Library
  By: Daniel Bernal
  Date: Feb 2, 2021
  Hardware Connections:
  SCD30 with pins soldered an connected to SDA and SCL
  TM1637 board connected at pin A2 and A0
  Buzzer connected at pin 8
  Button connected at pin 12
  Leyendo CO2, humedad y temeratura desde el SCD30 marca Sensirion, visualización en el 7 segmentos TM1637, chicharra y rutina de calibración.
  Basado en la libreria SparkFun_SCD30_Arduino_Library
  Por: Daniel Bernal
  Fecha: Feb 2, 2021
  Conección de Hardware:
  Sensor SCD30 con pines soldados y conectados a SDA y SCL
  Board TM1637 conectada a los pines A2 y A0
  Chicharra conectada al pin 8
  Boton o cable conectado a pin 12
*/

#include <Wire.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include "SparkFun_SCD30_Arduino_Library.h"
#include <avr/wdt.h>

const byte PIN_CLK = 9;   // define CLK pin (any digital pin)
const byte PIN_DIO = 8;   // define DIO pin (any digital pin)
const byte BUTTON = 2;   // define DIO pin (any digital pin)
const byte BUZZER = 11;
unsigned int CO2 = 0;
unsigned int ConnRetry = 0;

SevenSegmentExtended display(PIN_CLK, PIN_DIO);
SCD30 airSensor;

void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  Serial.begin(115200);
  Serial.println("Start SCD30 lecture");
  Wire.begin();

  display.begin();            // initializes the display
  display.setBacklight(100);  // set the brightness to 100 %

  // Turn off calibration and Sensor connection test

  while ((airSensor.begin(Wire, false) == false) && (ConnRetry < 5))
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

  display.print("good");                   // display loop counter
  Serial.println("SCD30 read OK");
  delay(5000);

  // Turn off calibration
  Serial.print("Auto calibration set to ");
  if (airSensor.getAutoSelfCalibration() == true)
    Serial.println("true");
  else
    Serial.println("false");

  //
  airSensor.setTemperatureOffset((float) 0);    // set for x degrees Temperature offset
  airSensor.setAmbientPressure(753); //Current ambient pressure in mBar: 700 to 1200, Bogota is 751 to 754 mbar (2600 m heigh above the sea level)
  airSensor.setMeasurementInterval(2); //Change number of seconds between measurements: 2 to 1800

  // Preheat routine: min 20 seconds for SCD30
  display.print("HEAT");
  Serial.print("Preheat: ");
  delay (3000);

  for (int i = 20; i > -1; i--) { // loop from 0 to 20
    display.printNumber(i);
    Serial.println(i);
    delay(1000);
  }

  display.clear();
  delay(10);
  display.print("CO2-");
  delay(5000);
}

void loop()
{
  if (airSensor.dataAvailable())
  {
    CO2 = airSensor.getCO2();
    Serial.print("CO2(ppm): ");
    Serial.println(CO2);

    Serial.print("Temp(°C): ");
    Serial.println(airSensor.getTemperature(), 1);

    Serial.print("Humidity(%): ");
    Serial.println(airSensor.getHumidity(), 1);

    Serial.println();
    display.clear();
    delay(10);
    display.printNumber(CO2);

    // Alarm if CO2 is greater than 1000

    if (CO2 > 1000) {
      Beep();
    }

    // If you push the button see the humidity measured by SCD30

    if (digitalRead(BUTTON) == LOW) {
      delay (10);
      if (digitalRead(BUTTON) == LOW) {
        delay (10);
        if (digitalRead(12) == LOW) {
          display.clear();
          display.print("h_" + String(int(airSensor.getHumidity())));
          //display.print(int(airSensor.getHumidity()));
          delay (1000);
        }
      }
    }

    // First. put the sensor outside in a close box and wait 5 minutes, this is the reference of 400ppm
    // Open and push the button more than 5 seconds
    // Close the box and the sensor enter to the calibration process
    // At the end the sensor receives the order of calibration to 400ppm

    ///Esto se puede hacer mejor///

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
            CO2 = airSensor.getCO2();
            Serial.print(i);
            Serial.print(" CO2(ppm): ");
            Serial.println(CO2);
            delay(1000);
          }
          Calibration();
        }
      }
    }

  }
  else {
    Serial.println("Waiting for new data");
    delay(2000);
  }
}

void Beep()
{
  tone(BUZZER, 900);
  delay(330);
  noTone(BUZZER);
}

void Calibration()
{
  airSensor.setForcedRecalibrationFactor(400);
  Serial.print("Resetting forced calibration factor to : 400");
  display.print("done");
  delay(5000);
}

void softwareReset( uint8_t prescaller) {
  wdt_enable( prescaller);
  while (1) {}
}
