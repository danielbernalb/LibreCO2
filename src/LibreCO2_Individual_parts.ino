/*
  Reading CO2 from the SCD30, MHZ14 or 19, CM1106 or the SenseAir S8.
  By: Daniel Bernal
  Date: August 23, 2021
  Hardware Connections:
  Please see the instructions in github (https://github.com/danielbernalb/LibreCO2) or aireciudadano (https://aireciudadano.com/guia-de-construccion-medidor-libreco2/)

  Leyendo CO2 desde el sensor SCD30, MHZ14 o 19, CM1106 o SenseAir S8.
  Por: Daniel Bernal
  Fecha: Agosto 23, 2021
  Conección de Hardware:
  Favor revisar las instrucciones en github (https://github.com/danielbernalb/LibreCO2/blob/main/INSTRUCCIONES%20en%20Espa%C3%B1ol.md) o aireciudadano (https://aireciudadano.com/guia-de-construccion-medidor-libreco2/)
  Rev 147
*/

// ***************************************************************************
// ***************************************************************************
// MANDATORY: UNCOMMENT YOUR CO2 SENSOR MODEL!!!

//#define SCD30        // Sensirion SCD30
//#define MHZ14_9      // Winsen MHZ14 or 19
//#define CM1106       // Cubic CM1106
#define SenseAir_S8 // SenseAir S8

// UNCOMMENT FOR DEBUG MODE
//#define DEBUG

// ***************************************************************************
// ***************************************************************************

#include <SoftwareSerial.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include <EEPROM.h>
#include <CRCx.h> //https://github.com/hideakitai/CRCx
#include <avr/wdt.h>

const byte RevVersion = 147; // Firmware version 23 aug 2021

const byte PIN_TX = 6;  // define TX pin to Sensor
const byte PIN_RX = 7;  // define RX pin to Sensor
const byte PIN_CLK = 9;
const byte PIN_DIO = 8;

const byte BUTTON_CALI = A0;
const byte BUTTON_BEEP = A3;
const byte BUTTON_ALTI = 2;
const byte BUZZER = 11;
const byte LED = 13;

const byte addressBEEP = 0; // EEPROM address 0
const byte addressALTI = 1; // EEPROM address 1

const byte MB_PKT_4 = 4;   //CM1106 receive Packet Size
const byte MB_PKT_6 = 6;   //CM1106 receive Packet Size
const byte MB_PKT_7 = 7;   //MODBUS Packet Size
const byte MB_PKT_8 = 8;   //MODBUS Packet Size
const byte MB_PKT_9 = 9;   //MHZ14_9 Packet Size
const byte MB_PKT_10 = 10; //CM1106 send Packet Size
const byte MB_PKT_17 = 17; // MODBUS Packet Size

// SCD30 MODBUS commands
const byte cmdConM[MB_PKT_8] = {0x61, 0x06, 0x00, 0x36, 0x00, 0x00, 0x60, 0x64}; // SCD30 Trigger continuous measurement with no ambient pressure compensation
const byte cmdSetM[MB_PKT_8] = {0x61, 0x06, 0x00, 0x25, 0x00, 0x02, 0x10, 0x60}; // SCD30 Set measurement interval 2 seconds
const byte cmdAuto[MB_PKT_8] = {0x61, 0x06, 0x00, 0x3A, 0x00, 0x00, 0xA0, 0x67}; // SCD30 Deactivate Automatic Self-Calibration
const byte cmdRead[MB_PKT_8] = {0x61, 0x03, 0x00, 0x28, 0x00, 0x06, 0x4C, 0x60}; // SCD30 Read CO2
const byte cmdCali[MB_PKT_8] = {0x61, 0x06, 0x00, 0x39, 0x01, 0x90, 0x51, 0x9B}; // SCD30 Calibrate to 400pm

const byte cmdGalt[MB_PKT_8] = {0x61, 0x03, 0x00, 0x38, 0x00, 0x01, 0x0C, 0x67}; // SDC30 Get Altitude compensation

// MHZ14_9 serial coomands
const byte cmdReMH[MB_PKT_9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // MHZ14_9 Read command
const byte cmdCalM[MB_PKT_9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78}; // MHZ14_9 Calibrate ZERO point
const byte cmdOFFM[MB_PKT_9] = {0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86}; // MHZ14_9 Disables Automatic Baseline Correction

// CM1106 serial commands
const byte cmdReCM[MB_PKT_4] = {0x11, 0x01, 0x01, 0xED};                                      // CM1106 Read CO2
const byte cmdOFFa[MB_PKT_10] = {0x11, 0x07, 0x10, 0x64, 0x02, 0x07, 0x01, 0x90, 0x64, 0x76}; // CM1106 Command Close ABC
const byte cmdCalC[MB_PKT_6] = {0x11, 0x03, 0x03, 0x01, 0x90, 0x58};                          // CM1106 Command Calibration of CO2 Concentration to 400ppm
const byte cmdOKqu[MB_PKT_4] = {0x16, 0x01, 0x10, 0xD9};                                      // CM1106 response OK
const byte cmdOKca[MB_PKT_4] = {0x16, 0x01, 0x03, 0xE6};                                      // CM1106 response OK

// SenseAir S8 MODBUS commands
const byte cmdReSA[MB_PKT_8] = {0xFE, 0X04, 0X00, 0X03, 0X00, 0X01, 0XD5, 0XC5}; // SenseAir Read CO2
const byte cmdOFFs[MB_PKT_8] = {0xFE, 0x06, 0x00, 0x1F, 0x00, 0x00, 0xAC, 0x03}; // SenseAir Close ABC
const byte cmdCal1[MB_PKT_8] = {0xFE, 0x06, 0x00, 0x00, 0x00, 0x00, 0x9D, 0xC5}; // SenseAir Calibration 1
const byte cmdCal2[MB_PKT_8] = {0xFE, 0x06, 0x00, 0x01, 0x7C, 0x06, 0x6C, 0xC7}; // SenseAir Calibration 2
const byte cmdCal3[MB_PKT_8] = {0xFE, 0x03, 0x00, 0x00, 0x00, 0x01, 0x90, 0x05}; // SenseAir Calibration 3
const byte cmdCalR[MB_PKT_7] = {0xFE, 0x03, 0x02, 0x00, 0x20, 0xAD, 0x88};       // SenseAir Calibration Response

static byte response[MB_PKT_8] = {0};

bool isLongPressCALI = false;
bool isLongPressBEEP = false;
bool isLongPressALTI = false;
bool ExitCali = false;

byte VALbeep = 0;
int VALDIS = 0;
byte VALalti = 0;
byte ConnRetry = 0;
int CO2 = 0;
int CO2value;
unsigned int CO2temp = 0;
unsigned int crc_cmd;
unsigned int delayIN = 0;
unsigned long StartPress_ms = 0;
float CO2cor;
float hpa;

const int LongPress_ms = 5000; // 5s button timeout


SevenSegmentExtended display(PIN_CLK, PIN_DIO);
SoftwareSerial co2sensor(PIN_RX, PIN_TX);
#ifdef SCD30
#define BAUDRATE 19200 // Device to SCD30 Serial baudrate (should not be changed)
#else
#define BAUDRATE 9600 // Device to SenseAir S8, MHZ19 and CM1106 Serial baudrate (should not be changed)
#endif

void setup()
{
  pinMode(BUTTON_CALI, INPUT_PULLUP);
  pinMode(BUTTON_BEEP, INPUT_PULLUP);
  pinMode(BUTTON_ALTI, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(4, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A5, LOW);
  digitalWrite(LED, LOW);

  Serial.begin(9600);

  display.begin();           // initializes the display
  display.setBacklight(100); // set the brightness to 100 %
  delay(100);

  Serial.print(F("Firmware version: "));
  Serial.println(RevVersion);

  display.clear();
  display.print("Lco2");
  Serial.println(F("Start LibreCO2"));

#ifdef SCD30
  Serial.println(F("Start SCD30 Modbus lecture"));
#endif
#ifdef MHZ14_9
  Serial.println(F("Start MH-Z14 or MH-Z19 lecture"));
#endif
#ifdef CM1106
  Serial.println(F("Start CM1106 lecture"));
#endif
#ifdef SenseAir_S8
  Serial.println(F("Start SenseAir S8 lecture"));
#endif
  co2sensor.begin(BAUDRATE); // Sensor serial start
  delay(4000);

  VALbeep = EEPROM.read(addressBEEP);
  displayVALbeep(1000);
  delay(1500);

  VALalti = EEPROM.read(addressALTI);
  displayVALalti();
  delay(1500);

  // hPa calculation
#ifndef SCD30
  hPaCalculation();
#endif

  // Sensor init
#ifdef SCD30
  CO2iniSCD30();
#endif
#ifdef MHZ14_9
  CO2iniMHZ19();
#endif
#ifdef CM1106
  CO2iniCM1106();
#endif
#ifdef SenseAir_S8
  CO2iniSenseAir();
#endif

  // Preheat routine: min 30 seconds for SCD30 & CM1106 & SenseAir S8, 180 seconds for MH-Z14 or 19
  Serial.print(F("Preheat: "));
#ifdef MHZ14_9
  for (int i = 180; i > -1; i--)
#else
  for (int i = 30; i > -1; i--)
#endif
  { // Preheat from 0 to 30 or to 180
    display.clear();
    display.print("HEAT");
    delay(1000);
    display.clear();
    display.printNumber(i);
    Serial.println(i);
    delay(1000);
    i--;
  }
  Serial.print(F("Start measurements compensated by Altitude: "));
  Serial.print(VALalti * 50);
  Serial.println(" m");
  display.clear();
  display.print("CO2-");
  delay(5000);
}

void loop()
{
  CO2 = 0;
#ifdef SCD30
  CO2 = co2SCD30();
#endif
#ifdef MHZ14_9
  CO2value = co2MHZ14_9();
  if (CO2value != 0)
  {
    CO2cor = float(CO2value) + (0.016 * ((1013 - float(hpa)) / 10) * (float(CO2value) - 400)); // Increment of 1.6% for every hPa of difference at sea level
    CO2 = round(CO2cor);
  }
#ifdef DEBUG
  DebugCO2val();
#endif
#endif
#ifdef CM1106
  CO2value = co2CM1106();
  if (CO2value != 0)
  {
    CO2cor = float(CO2value) + (0.016 * ((1013 - float(hpa)) / 10) * (float(CO2value) - 400)); // Increment of 1.6% for every hPa of difference at sea level
    CO2 = round(CO2cor);
  }
#ifdef DEBUG
  DebugCO2val();
#endif
#endif
#ifdef SenseAir_S8
  CO2value = co2SenseAir();
  if (CO2value != 0)
  {
    CO2cor = float(CO2value) + (0.016 * ((1013 - float(hpa)) / 10) * (float(CO2value) - 400)); // Increment of 1.6% for every hPa of difference at sea level
    CO2 = round(CO2cor);
  }
  else
    CO2 = 0;
#ifdef DEBUG
  DebugCO2val();
#endif
#endif

  if (CO2 > 0)
  {
#ifdef DEBUG
    Serial.print(F("CO2(ppm): "));
#else
    Serial.print(" ");
#endif
    Serial.println(CO2);
    display.clear();
    delay(10);
    display.printNumber(CO2);
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("ERROR CO2=<0"));
#endif
    display.clear();
    delay(10);
    display.print("----");
  }

  // Alarm if CO2 is greater than VALDIS

  if (CO2 >= VALDIS && VALDIS != 1450)
  {
    Beep();

    digitalWrite(LED, !digitalRead(LED));
  }

  check_calmode_active();
#ifdef SCD30
  delay(1971); // for SCD30 2 seconds
#endif
#ifdef MHZ14_9
  delay(1978); // for MHZ14_9 2 seconds
  delayMicroseconds(570);
#endif
#ifdef CM1106
  delay(1978); // for CM1106 2 seconds
#endif
#ifdef SenseAir_S8
  delay(3967); // for SenseAir S8 4 seconds
#endif
}

//Routine Bad connection

void BadConn()
{
  Serial.println("Air sensor not detected. Please check wiring... Try# " + String(ConnRetry));

  if (ConnRetry > 1)
  {
    display.clear();
    delay(20);
    display.print("bad");
  }
  delay(2500);
  ConnRetry++;
}

// Beep 900Hhz
void Beep()
{
  tone(BUZZER, 900);
  delay(330);
  noTone(BUZZER);
}

void Done()
{
  Serial.println(F("done"));
  display.clear();
  delay(10);
  display.print("done");
}

void Failed()
{
  Serial.println(F("failed"));
  display.clear();
  delay(10);
  display.print("fail");
}

void DebugCO2val()
{
  Serial.print("hpa: ");
  Serial.print(hpa);
  Serial.print(" CO2real: ");
  Serial.print(CO2value);
  Serial.print(" CO2adjust: ");
  Serial.println(CO2cor);
}

//Done or failed revision routine
void CheckResponse(uint8_t *a, uint8_t *b, uint8_t len_array_cmp)
{
  bool check_match = false;
  for (int n = 0; n < len_array_cmp; n++)
  {
    if (a[n] != b[n])
    {
      check_match = false;
      break;
    }
    else
      check_match = true;
  }

  if (check_match)
  {
    Done();
  }
  else
  {
    Failed();
  }
}

// Press button routine for Calibration

// First put the sensor outside in a close box and wait 5 minutes, this is the reference of 400ppm
// Open and push the button more than 5 seconds
// Close the box and the sensor enter to the calibration process
// At the end the sensor receives the order of calibration to 400ppm

void check_calmode_active()
{
  unsigned long currentTime_ms = millis();

  // Test CALI button

  if (digitalRead(BUTTON_CALI) == LOW)
  {
    if (isLongPressCALI)
    {
      if (currentTime_ms > (StartPress_ms + LongPress_ms))
      {
        ExitCali = false;
        display.clear();
        delay(100);

#ifdef MHZ14_9
        Serial.println(F("Start calibration process: 1200 seconds of 400 ppm stable"));
        for (int i = 1200; i > -1; i--)
#else
        Serial.println(F("Start calibration process: 300 seconds of 400 ppm stable"));
        for (int i = 300; i > -1; i--)
#endif
        { // loop from 0 to 300
          
          display.clear();
          display.print("CAL-");
          delay(1000);
          Serial.print(i);
          Serial.print(" ");

#ifdef SCD30
          CO2 = co2SCD30();
#endif
#ifdef MHZ14_9
          CO2 = co2MHZ14_9();
#endif
#ifdef CM1106
          CO2 = co2CM1106();
#endif
#ifdef SenseAir_S8
          CO2 = co2SenseAir();
#endif
          Serial.print(F("CO2(ppm): "));
          Serial.println(CO2);
          display.clear();
          display.printNumber(i);
          delay(1000);
          i--;

          if (digitalRead(BUTTON_BEEP) == LOW)
          {
            delay(50);
            if (digitalRead(BUTTON_BEEP) == LOW)
            {
              i = 0;
              Serial.println(F("Calibration process canceled"));
              ExitCali = true;
              Failed();
              delay(2000);
              break;
            }
          }
        }
        if (ExitCali == false)
        {
#ifdef SCD30
          CalibrationSCD30();
#endif
#ifdef MHZ14_9
          CalibrationMHZ19();
#endif
#ifdef CM1106
          CalibrationCM1106();
#endif
#ifdef SenseAir_S8
          CalibrationSenseAir();
#endif

          isLongPressCALI = false;
          Done();
          delay(4000);
        }
        ExitCali = false;
      }
    }
    else
    {
      StartPress_ms = millis();
      isLongPressCALI = true;
      Serial.println(F("Button CALI has been pressed, hold 5s more to start calibration"));
    }
  }
  else
  {
    isLongPressCALI = false;
  }

  // Test BEEP button

  if (digitalRead(BUTTON_BEEP) == LOW)
  {
    if (isLongPressBEEP)
    {
      if (currentTime_ms > (StartPress_ms + LongPress_ms))
      {
        Serial.println(F("Routine BEEP change"));
        displayVALbeep(1500);

        while (digitalRead(BUTTON_ALTI) == HIGH)
        {
          delay(10);
          if (digitalRead(BUTTON_BEEP) == LOW)
          {
            delay(50);
            if (digitalRead(BUTTON_BEEP) == LOW)
            {
              VALbeep++;
              if (VALbeep > 15 || VALbeep == 0xFF)
                VALbeep = 0;
              displayVALbeep(0);
              delay(750);
            }
          }
        }
        delay(50);
        if (digitalRead(BUTTON_ALTI) == LOW)
        {
          EEPROM.write(addressBEEP, VALbeep);
          isLongPressBEEP = false;
          Serial.print(F("New Beep Value: "));
          if (VALDIS == 1450)
            Serial.println(F("OFF"));
          else
            Serial.println(VALDIS);
          Done();
          delay(2000);
        }
        else
        {
          Serial.println(F("no change in BEEP value"));
          Failed();
        }
      }
    }
    else
    {
      StartPress_ms = millis();
      isLongPressBEEP = true;
      Serial.println(F("Button BEEP has been pressed, hold 5s more to start BEEP level change routine"));
    }
  }

  // Test ALTITUDE button

  if (digitalRead(BUTTON_ALTI) == LOW)
  {
    if (isLongPressALTI)
    {
      if (currentTime_ms > (StartPress_ms + LongPress_ms))
      {
        Serial.println(F("Routine ALTITUDE change"));
        displayVALalti();
        delay(500);

        while (digitalRead(BUTTON_CALI) == HIGH)
        {
          delay(10);
          if (digitalRead(BUTTON_ALTI) == LOW)
          {
            delay(50);
            if (digitalRead(BUTTON_ALTI) == LOW)
            {
              VALalti++;
              if (VALalti > 80 || VALalti == 0xFF)
                VALalti = 0;
              Serial.print(F("ALTITUDE level: "));
              Serial.print(VALalti * 50);
              Serial.println(" m");
              display.clear();
              display.printNumber(VALalti * 50);
              delay(350);
            }
          }
          if (digitalRead(BUTTON_BEEP) == LOW)
          {
            delay(50);
            if (digitalRead(BUTTON_BEEP) == LOW)
            {
              if (VALalti == 0 || VALalti == 0xFF)
                VALalti = 81;
              VALalti--;
              Serial.print(F("ALTITUDE level: "));
              Serial.print(VALalti * 50);
              Serial.println(" m");
              display.clear();
              display.printNumber(VALalti * 50);
              delay(350);
            }
          }
        }
        delay(50);
        if (digitalRead(BUTTON_CALI) == LOW)
        {
          EEPROM.write(addressALTI, VALalti);
          isLongPressALTI = false;

#ifdef SCD30
          byte cmdSalt[MB_PKT_8] = {0};
          byte cmdTemp[6] = {0};
          byte resALTI[7] = {0};

          uint16_t CRC16 = 0;
          uint16_t ALT16 = VALalti * 50;

          cmdTemp[0] = 0x61;
          cmdTemp[1] = 0x06;
          cmdTemp[2] = 0x00;
          cmdTemp[3] = 0x38;

          cmdTemp[4] = ALT16 / 256;
          cmdTemp[5] = ALT16 % 256;

          CRC16 = crcx::crc16(cmdTemp, 6);

          cmdSalt[0] = 0x61;
          cmdSalt[1] = 0x06;
          cmdSalt[2] = 0x00;
          cmdSalt[3] = 0x38;
          cmdSalt[4] = ALT16 / 256;
          cmdSalt[5] = ALT16 % 256;
          cmdSalt[6] = CRC16 % 256;
          cmdSalt[7] = CRC16 / 256;

          //Altitude compensation value
          co2sensor.write(cmdSalt, MB_PKT_8);
          co2sensor.readBytes(response, MB_PKT_8);

          Serial.print(F("SCD30 Altitude compensation setted: "));
          CheckResponse(cmdSalt, response, MB_PKT_8);
          delay(2000);

          co2sensor.write(cmdGalt, MB_PKT_8);
          co2sensor.readBytes(resALTI, 7);

          Serial.print(F("Altitude value for SCD30: "));
          VALalti = ((256 * resALTI[3]) + resALTI[4]) / 50;
          display.clear();
          display.print("ALTI");
          delay(1000);
          display.clear();
          display.printNumber(VALalti * 50);
          Serial.print(VALalti * 50);
          Serial.println(" m");
#else
          Serial.print(F("New Altitude Value: "));
          display.clear();
          display.print("ALTI");
          delay(1000);
          display.clear();
          display.printNumber(VALalti * 50);
          Serial.print(VALalti * 50);
          Serial.println(" m");
          delay(1000);
          hPaCalculation();
          Done();
#endif
          delay(1000);
        }
        else
        {
          Serial.println(F("no change in ALTITUDE value"));
          Failed();
        }
      }
    }
    else
    {
      StartPress_ms = millis();
      isLongPressALTI = true;
      Serial.println(F("Button ALTITUDE has been pressed, hold 5s more to start ALTITUDE change routine"));
    }
  }
  else
  {
    isLongPressALTI = false;
  }
}

// BEEP value display

void displayVALbeep(unsigned int delayIN)
{
  if (VALbeep == 0xFF)
    VALbeep = 0;
  VALDIS = 1000 + (VALbeep * 50);
  if (VALDIS > 1450)
  {
    VALDIS = VALDIS - 800;
  }
  Serial.print(F("BEEP level: "));
  display.clear();
  display.print("BEEP");
  delay(delayIN);
  if (VALDIS == 1450)
  {
    display.clear();
    display.print("OFF");
    Serial.println(F("OFF"));
  }
  else
  {
    display.printNumber(VALDIS);
    Serial.println(VALDIS);
  }
}

// ALTITUDE value display

void displayVALalti()
{
  if (VALalti == 0xFF)
    VALalti = 0;

  if (VALalti > 80)
    VALalti = 0;

#ifdef SCD30

  byte resALTI[7] = {0};

  memset(response, 0, MB_PKT_8);
  co2sensor.write(cmdGalt, MB_PKT_8);
  co2sensor.readBytes(resALTI, 7);
  Serial.print(F("Altitude value readed from SCD30: "));
  VALalti = ((256 * resALTI[3]) + resALTI[4]) / 50;
#else
  Serial.print(F("Altitude value readed from memory: "));
#endif

  display.clear();
  display.print("ALTI");
  delay(1000);
  display.clear();
  display.printNumber(VALalti * 50);
  Serial.print(VALalti * 50);
  Serial.println(" m");
}

// Calculate of Atmospheric pressure

void hPaCalculation()
{
  hpa = 1013 - 5.9 * float(VALalti) + 0.011825 * float(VALalti) * float(VALalti); // Cuadratic regresion formula obtained PA (hpa) from high above the sea
  Serial.print(F("Atmospheric pressure calculated by the sea level inserted (hPa): "));
  Serial.println(hpa);
}

//Software RESET

void softwareReset(uint8_t prescaller)
{
  delay(2000);
  Serial.println(F("RESET..."));
  wdt_enable(prescaller);
  while (1)
  {
  }
}

// ***************************************************************************
// ***************************************************************************
// SCD30 routines

#ifdef SCD30

// Initialice SCD30
void CO2iniSCD30()
{
  //Start continious measurement
  co2sensor.write(cmdConM, MB_PKT_8);
  delay(5);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print(F("Continious measurement: "));
  CheckResponse(cmdConM, response, MB_PKT_8);
  delay(1000);

  //Set measurement interval 2 seconds
  co2sensor.write(cmdSetM, MB_PKT_8);
  delay(5);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print(F("Interval 2 seconds: "));
  CheckResponse(cmdSetM, response, MB_PKT_8);
  delay(1000);

  //Deactivate Automatic Self-Calibration
  co2sensor.write(cmdAuto, MB_PKT_8);
  delay(5);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print(F("Deactivate Automatic Self-Calibration SCD30: "));
  CheckResponse(cmdAuto, response, MB_PKT_8);
  delay(2000);

  while (co2SCD30() == 0 && (ConnRetry < 5))
  {
    BadConn();
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println(F(" Air sensor detected SCD30 Modbus"));
  display.clear();
  delay(10);
  display.print("good");
  delay(2500);
  Serial.println(F("SCD30 read OK"));
  delay(4000);
}

// CO2 lecture SCD30
int co2SCD30()
{
  byte responseSCD[MB_PKT_17] = {0};
  memset(responseSCD, 0, MB_PKT_17);

  union BYTE_FLOAT_CO2
  {
    byte uByte[4];
    float uCO2;
  } u;

  co2sensor.write(cmdRead, MB_PKT_8);
  co2sensor.readBytes(responseSCD, MB_PKT_17);

#ifdef DEBUG
  Serial.print(F("Read SCD30: "));
#endif

  // CRC Routine
  u.uByte[0] = responseSCD[6];
  u.uByte[1] = responseSCD[5];
  u.uByte[2] = responseSCD[4];
  u.uByte[3] = responseSCD[3];

  if (u.uCO2 != 0)
  {
    crc_cmd = crcx::crc16(responseSCD, 15);
    if (responseSCD[15] == lowByte(crc_cmd) && responseSCD[16] == highByte(crc_cmd))
    {
#ifdef DEBUG
      Serial.println(" OK");
#else
      Serial.print("OK");
#endif
      return (u.uCO2);
    }
    else
    {
      CO2temp = u.uCO2;

      if (CO2temp - CO2 < 10 && CO2temp - CO2 > -10)
      {
#ifdef DEBUG
        Serial.println(" OK");
#else
        Serial.print("OKR");
#endif
        return (u.uCO2);
      }
      else
      {
#ifdef DEBUG
        Serial.println(F("FAIL CRC CO2 last"));
#else
        Serial.println(F("FAIL1"));
#endif
        return CO2;
      }
    }
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("FAIL CRC CO2 continuously"));
#else
    Serial.println(F("FAIL3"));
#endif
    //return CO2;
    display.clear();
    display.print("----");
    return 0;
  }
}

// Calibration routine SCD30
void CalibrationSCD30()
{
  delay(100);

  //Calibration set to 400ppm
  co2sensor.write(cmdCali, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print(F("Resetting calibration to 400: "));
  CheckResponse(cmdCali, response, MB_PKT_8);
}
#endif

// MHZ14_9 routines

#ifdef MHZ14_9

// Initialice MHZ19
void CO2iniMHZ19()
{
  //Deactivate Automatic Self-Calibration
  co2sensor.write(cmdOFFM, MB_PKT_9);
  Serial.println(F("Deactivate Automatic Self-Calibration MHZ14_9: done"));
  delay(2000);

  while (co2MHZ14_9() == 0 && (ConnRetry < 5))
  {
    BadConn();
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println(F(" Air sensor detected MHZ14_9"));
  display.clear();
  delay(10);
  display.print("good");
  delay(2500);
  Serial.println(F("MHZ14_9 read OK"));
  delay(4000);
}

//Read MHZ14 or 19
int co2MHZ14_9()
{
  byte responseMH[MB_PKT_9] = {0};
  memset(responseMH, 0, MB_PKT_9);
  co2sensor.write(cmdReMH, MB_PKT_9);
  co2sensor.readBytes(responseMH, MB_PKT_9);
  CO2 = (256 * responseMH[2]) + responseMH[3];

#ifdef DEBUG
  Serial.print(F("Read MHZ14_9: "));
#endif

  if (CO2 != 0)
  {
    byte crc = 0;
    for (int i = 1; i < 8; i++)
    {
      crc += responseMH[i];
    }
    crc = 256 - crc;

    if ((responseMH[1] == 0x86) && (responseMH[8] == crc))
    {
#ifdef DEBUG
      Serial.println(F("OK DATA"));
#else
      Serial.print("OK");
#endif
      return CO2;
    }
    else
    {
      while (co2sensor.available() > 0)
        char t = co2sensor.read(); // Clear serial input buffer;

#ifdef DEBUG
      Serial.println(F("FAIL CRC_CO2"));
#else
      Serial.println(F("FAIL"));
#endif
      display.clear();
      display.print("----");
      return 0;
    }
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("FAILED"));
#else
    Serial.println(F("FAIL"));
#endif
    display.clear();
    delay(10);
    display.print("----");
    return 0;
  }
}

// Calibration routine MHZ14_9
void CalibrationMHZ19()
{
  delay(100);

  //Calibration set to 400ppm
  co2sensor.write(cmdCalM, MB_PKT_9);
  Serial.print(F("Resetting forced calibration factor to 400: "));
}
#endif

// CM1106 routines

#ifdef CM1106

// Initialize CM1106
void CO2iniCM1106()
{
  byte responseCM[MB_PKT_4] = {0};
  // Stop Autocalibration
  co2sensor.write(cmdOFFa, MB_PKT_10);
  co2sensor.readBytes(responseCM, MB_PKT_4);

  Serial.print(F("Deactivate Automatic Self-Calibration CM1106: "));
  CheckResponse(cmdOKqu, responseCM, MB_PKT_4);
  delay(1000);

  // Connection test
  while (co2CM1106() == 0 && (ConnRetry < 5))
  {
    BadConn();
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println(F(" Air sensor detected CM1106"));
  display.clear();
  delay(10);
  display.print("good");
  Serial.println(F("CM1106 read OK"));
  delay(4000);
}

//Read CM1106
int co2CM1106()
{
  static byte responseCM[MB_PKT_8] = {0};
  memset(responseCM, 0, MB_PKT_8);
  co2sensor.write(cmdReCM, MB_PKT_4);
  co2sensor.readBytes(responseCM, MB_PKT_8);
  CO2 = (256 * responseCM[3]) + responseCM[4];

#ifdef DEBUG
  Serial.print(F("Read CM1106: "));
#endif

  if (CO2 != 0)
  {

    int crc = 0;
    for (int i = 0; i < 7; i++)
      crc += responseCM[i];
    crc = 256 - crc % 256;
    if ((responseCM[0] == 0x16) && (responseCM[7] == crc))
    {
#ifdef DEBUG
      Serial.println(F("OK DATA"));
#else
      Serial.print("OK");
#endif
      return CO2;
    }
    else
    {
      while (co2sensor.available() > 0)
        char t = co2sensor.read(); // Clear serial input buffer;

#ifdef DEBUG
      Serial.println(F("FAIL CRC_CO2"));
#else
      Serial.println(F("FAIL"));
#endif
      display.clear();
      display.print("----");
      return 0;
    }
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("FAILED"));
#else
    Serial.println(F("FAIL"));
#endif
    display.clear();
    delay(10);
    display.print("----");
    return 0;
  }
}

// Calibration routine CM1106
void CalibrationCM1106()
{
  byte responseCM[MB_PKT_4] = {0};
  delay(100);

  //Calibration set to 400ppm
  co2sensor.write(cmdCalC, MB_PKT_6);
  co2sensor.readBytes(responseCM, MB_PKT_4);
  Serial.print(F("Resetting forced calibration factor to 400: "));
  CheckResponse(cmdOKca, responseCM, MB_PKT_4);
}
#endif

// SenseAir S8 routines

#ifdef SenseAir_S8

// Initialice SenseAir S8
void CO2iniSenseAir()
{
  //Deactivate Automatic Self-Calibration
  co2sensor.write(cmdOFFs, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);
  Serial.print(F("Deactivate Automatic Self-Calibration SenseAir S8: "));
  CheckResponse(cmdOFFs, response, MB_PKT_8);
  delay(2000);

  while (co2SenseAir() == 0 && (ConnRetry < 5))
  {
    BadConn();
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println(F(" Air sensor detected AirSense S8 Modbus"));

  display.clear();
  delay(10);
  display.print("good");
  //delay(2500);
  Serial.println(F("SenseAir read OK"));
  delay(4000);
}

// CO2 lecture SenseAir S8
int co2SenseAir()
{
  static byte responseSA[MB_PKT_7] = {0};
  memset(responseSA, 0, MB_PKT_7);
  co2sensor.write(cmdReSA, MB_PKT_8);
  co2sensor.readBytes(responseSA, MB_PKT_7);
  CO2 = (256 * responseSA[3]) + responseSA[4];

#ifdef DEBUG
  Serial.print(F("Read SenseAir S8: "));
#endif

  if (CO2 != 0)
  {
    crc_cmd = crcx::crc16(responseSA, 5);
    if (responseSA[5] == lowByte(crc_cmd) && responseSA[6] == highByte(crc_cmd))
    {
#ifdef DEBUG
      Serial.println(F("OK DATA"));
#else
      Serial.print("OK");
#endif
      return CO2;
    }
    else
    {
      while (co2sensor.available() > 0)
        char t = co2sensor.read(); // Clear serial input buffer;

#ifdef DEBUG
      Serial.println(F("FAIL CRC_CO2"));
#else
      Serial.println(F("FAIL"));
#endif
      display.clear();
      display.print("----");
      return 0;
    }
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("FAILED"));
#else
    Serial.println(F("FAIL"));
#endif
    display.clear();
    delay(10);
    display.print("----");
    return 0;
  }
}

// Calibration routine SenseAir S8

void CalibrationSenseAir()
{
  byte responseSA[MB_PKT_7] = {0};
  delay(100);

  //Step 1 Calibration
  co2sensor.write(cmdCal1, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print(F("Calibration Step1: "));
  CheckResponse(cmdCal1, response, MB_PKT_8);
  delay(1000);

  //Step 2 Calibration
  co2sensor.write(cmdCal2, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print(F("Calibration Step2: "));
  CheckResponse(cmdCal2, response, MB_PKT_8);
  delay(4000);

  //Step 3 Calibration
  co2sensor.write(cmdCal3, MB_PKT_8);
  co2sensor.readBytes(responseSA, MB_PKT_7);

  Serial.print(F("Resetting forced calibration factor to 400: "));
  CheckResponse(cmdCalR, responseSA, MB_PKT_7);
}
#endif
