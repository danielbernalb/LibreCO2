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

#include <CRCx.h> //https://github.com/hideakitai/CRCx, install it from Library Manager as CRCx
#include <SoftwareSerial.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include <avr/wdt.h>

const byte PIN_TX = 6;  // define CLK pin (any digital pin)
const byte PIN_RX = 7;  // define DIO pin (any digital pin)
const byte PIN_CLK = 9; // define CLK pin (any digital pin)
const byte PIN_DIO = 8; // define DIO pin (any digital pin)
const byte BUTTON = 2;  // define DIO pin (any digital pin)
const byte BUZZER = 11;
const int MB_PKT_8 = 8; //MODBUS Packet Sizes
const int MB_PKT_17 = 17;

unsigned int CO2 = 0;
unsigned int ConnRetry = 0;
uint16_t crc_cmd;

union BYTE_FLOAT_CO2
{
  byte uByte[4];
  float uCO2;
} u;

static byte response[MB_PKT_8] = {0};
static byte responseval[MB_PKT_17] = {0};
const byte cmd1[MB_PKT_8] = {0x61, 0x06, 0x00, 0x36, 0x00, 0x00, 0x60, 0x64};     //Trigger continuous measurement with no ambient pressure compensation
const byte cmd2[MB_PKT_8] = {0x61, 0x06, 0x00, 0x25, 0x00, 0x02, 0x10, 0x60};     //Set measurement interval 2 seconds
const byte cmd3[MB_PKT_8] = {0x61, 0x06, 0x00, 0x3A, 0x00, 0x00, 0xA0, 0x67};     //Deactivate Automatic Self-Calibration
const byte cmd4[MB_PKT_8] = {0x61, 0x03, 0x00, 0x3A, 0x00, 0x01, 0xAD, 0xA7};     //Request Auto calibration status
const byte cmdRead[MB_PKT_8] = {0x61, 0x03, 0x00, 0x28, 0x00, 0x06, 0x4C, 0x60};  //Read CO2
const byte cmdCal[MB_PKT_8] = {0x61, 0x06, 0x00, 0x39, 0x01, 0x90, 0x51, 0x9B};  //Calibrate to 400pm


unsigned long LongPress_ms = 5000; // 5s button timeout
unsigned long StartPress_ms = 0;
bool isLongPress = false;

#define BAUDRATE 19200 // Device to SCD30 Serial baudrate (should not be changed)

SevenSegmentExtended display(PIN_CLK, PIN_DIO);
SoftwareSerial co2SCD(PIN_RX, PIN_TX);

void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  Serial.begin(115200);
  Serial.println("Start SCD30 Modbus lecture");
  display.begin();           // initializes the display
  display.setBacklight(100); // set the brightness to 100 %
  co2SCD.begin(BAUDRATE);    // (Uno example) device to SCD30 serial start
  delay(1000);

  //Trigger continuous measurement with no ambient pressure compensation

  co2SCD.write(cmd1, MB_PKT_8);
  co2SCD.readBytes(response, MB_PKT_8);

  Serial.println("Start continious measurement");
  CheckResponse(cmd1, response, MB_PKT_8);
  delay(1000);

  //Set measurement interval 2 seconds
  co2SCD.write(cmd2, MB_PKT_8);
  co2SCD.readBytes(response, MB_PKT_8);

  Serial.println("Set measurement interval 2 seconds ");
  CheckResponse(cmd2, response, MB_PKT_8);
  delay(1000);

  //Deactivate Automatic Self-Calibration
  co2SCD.write(cmd3, MB_PKT_8);
  co2SCD.readBytes(response, MB_PKT_8);

  Serial.println("Deactivate Automatic Self-Calibration ");

  //Request Auto calibration status
  co2SCD.write(cmd4, MB_PKT_8);
  co2SCD.readBytes(response, 7);

  Serial.print("Autocalibration set to: ");
  if (response[4] == 00)
  {
    Serial.print(" OFF");
    display.clear();
    delay(15);
    display.print("done");
  }
  else
  {
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
  delay(4000);

  // Preheat routine: min 30 seconds for SCD30
  display.clear();
  Serial.print("Preheat: ");
  for (int i = 30; i > -1; i--)
  { // Preheat from 0 to 30
    display.clear();
    display.print("HEAT");
    delay(1000);
    display.clear();
    display.printNumber(i);
    Serial.println(i);
    delay(1000);
    i--;
  }
  display.clear();
  display.print("CO2-");
  delay(5000);
}

void loop()
{
  CO2 = co2SCD30();

  if (CO2 > 0)
  {
    Serial.print("CO2(ppm): ");
    Serial.println(CO2);

    display.clear();
    delay(10);
    display.printNumber(CO2);
  }
  else
  {
    delay(10);
  }

  // Alarm if CO2 is greater than 1000

  if (CO2 > 1000)
  {
    Beep();
  }

  // First. put the sensor outside in a close box and wait 5 minutes, this is the reference of 400ppm
  // Open and push the button more than 5 seconds
  // Close the box and the sensor enter to the calibration process
  // At the end the sensor receives the order of calibration to 400ppm

  check_calmode_active();
  Serial.println("Waiting for new data");
  delay(2000);
}

// CO2 lecture
int co2SCD30()
{
  co2SCD.write(cmdRead, MB_PKT_8);
  co2SCD.readBytes(responseval, MB_PKT_17);

  Serial.print("Read Sensor: ");

  // CRC Routine
  crc_cmd = crcx::crc16(responseval, 15);
  if (responseval[15] == lowByte(crc_cmd) && responseval[16] == highByte(crc_cmd))
  {
    Serial.println("OK");
    u.uByte[0] = responseval[6];
    u.uByte[1] = responseval[5];
    u.uByte[2] = responseval[4];
    u.uByte[3] = responseval[3];
    return (u.uCO2);
  }
  else
  {
    Serial.println("FAIL");
    display.clear();
    display.print("fail");
  }
}

// Beep 900Hhz
void Beep()
{
  tone(BUZZER, 900);
  delay(330);
  noTone(BUZZER);
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
    Serial.println("done");
    display.clear();
    display.print("done");
  }
  else
  {
    Serial.println("failed");
    display.clear();
    display.print("fail");
  }
}

// Press button routine for Calibration
void check_calmode_active()
{
  unsigned long currentTime_ms = millis();

  if (digitalRead(BUTTON) == LOW)
  {
    if (isLongPress)
    {
      if (currentTime_ms > (StartPress_ms + LongPress_ms))
      {
        Serial.println("Start calibration process: 300 seconds of 400 ppm stable");
        display.clear();
        delay(100);
        for (int i = 300; i > -1; i--)
        { // loop from 0 to 300
          display.clear();
          display.print("CAL-");
          delay(1000);
          Serial.print(i);
          Serial.print(" ");
          CO2 = co2SCD30();
          Serial.print("CO2(ppm): ");
          Serial.println(CO2);
          display.clear();
          display.printNumber(i);
          delay(1000);
          i--;
        }
        Calibration();
        isLongPress = false;
      }
    }
    else
    {
      StartPress_ms = millis();
      isLongPress = true;
      Serial.println("Button has been pressed, hold 5s more to start calibration");
    }
  }
  else
  {
    isLongPress = false;
  }
}

// Calibration routine
void Calibration()
{
  delay(100);

  //Calibration set to 400ppm
  co2SCD.write(cmdCal, MB_PKT_8);
  co2SCD.readBytes(response, MB_PKT_8);
  
  Serial.print("Resetting forced calibration factor to 400: ");
  CheckResponse(cmdCal, response, MB_PKT_8);
  delay(4000);
}

//Software RESET

void softwareReset(uint8_t prescaller)
{
  Serial.println("RESET...");
  wdt_enable(prescaller);
  while (1)
  {
  }
}