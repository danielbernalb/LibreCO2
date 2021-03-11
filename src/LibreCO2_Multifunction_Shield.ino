/*
  Reading CO2 from the SCD30, visualize in TM1637 7 segments, buzzer, calibration routine.
  Routine of SCD30 lecture based on Modbus protocol
  By: Daniel Bernal
  Date: Feb 7, 2021

  Hardware Connections:
  SCD30 with pins to 7 and 6
  TM1637 board connected at pin 9 and 8
  Buzzer connected at pin 11
  Button for Calibration connected at pin 4
  Button for change the BEEP level at pin A3

  Leyendo CO2 desde el sensor SCD30 marca Cubic, visualización en el 7 segmentos TM1637, chicharra y rutina de calibración.
  Rutina de lectura del sensor basada en el respositorio agnunez/moco2
  Por: Daniel Bernal
  Fecha: Feb 7, 2021

  Conección de Hardware:
  Sensor CM1106 conectado a 7 y 6
  Board TM1637 conectada a los pines 9 y 8
  Chicharra conectada al pin 11
  Botón de Calibración o cable conectado a pin 4
  Botón de cambio del nivel del BEEP o chicharra conectado a pin A3
*/

// Uncomment your CO2 sensor
#define SCD30
//#define MHZ14_9
//#define CM1106

#include <CRCx.h> //https://github.com/hideakitai/CRCx
#include <SoftwareSerial.h>
//#include "SevenSegmentTM1637.h"
//#include "SevenSegmentExtended.h"
#include <TimerOne.h>
#include <MultiFuncShield.h>
#include "MHZ19.h" //https://github.com/WifWaf/MH-Z19
#include <EEPROM.h>
#include <avr/wdt.h>

const byte PIN_TX = 5;  // define TX pin to Sensor
const byte PIN_RX = 6;  // define RX pin to Sensor
const byte PIN_SEL = 9;  // define SEL pin for SCD30
const byte BUTTON_CALIB = A1;
const byte BUTTON_BEEP = A2;
const byte BUTTON_ENTER = A3;
const byte MB_PKT_8 = 8;   //MODBUS Packet Size
const byte MB_PKT_17 = 17; // MODBUS Packet Size
const byte MB_PKT_10 = 10; //CM1106 send Packet Size
const byte MB_PKT_6 = 6;   //CM1106 receive Packet Size
const byte MB_PKT_4 = 4;   //CM1106 receive Packet Size
const byte address = 0;    // EEPROM address 0

const byte cmdConM[MB_PKT_8] = {0x61, 0x06, 0x00, 0x36, 0x00, 0x00, 0x60, 0x64};              // SCD30 Trigger continuous measurement with no ambient pressure compensation
const byte cmdSetM[MB_PKT_8] = {0x61, 0x06, 0x00, 0x25, 0x00, 0x02, 0x10, 0x60};              // SCD30 Set measurement interval 2 seconds
const byte cmdAuto[MB_PKT_8] = {0x61, 0x06, 0x00, 0x3A, 0x00, 0x00, 0xA0, 0x67};              // SCD30 Deactivate Automatic Self-Calibration
const byte cmdRead[MB_PKT_8] = {0x61, 0x03, 0x00, 0x28, 0x00, 0x06, 0x4C, 0x60};              // SCD30 Read CO2
const byte cmdCali[MB_PKT_8] = {0x61, 0x06, 0x00, 0x39, 0x01, 0x90, 0x51, 0x9B};              // SCD30 Calibrate to 400pm
const byte cmdReCM[MB_PKT_4] = {0x11, 0x01, 0x01, 0xED};                                      // CM1106 Read
const byte cmdOFFa[MB_PKT_10] = {0x11, 0x07, 0x10, 0x64, 0x02, 0x07, 0x01, 0x90, 0x64, 0x76}; // CM1106 Command Close ABC
const byte cmdCalC[MB_PKT_6] = {0x11, 0x03, 0x03, 0x01, 0x90, 0x58};                          // CM1106 Command Calibration of CO2 Concentration to 400ppm
const byte cmdOKqu[MB_PKT_4] = {0x16, 0x01, 0x10, 0xD9};                                      // CM1106 response OK
const byte cmdOKca[MB_PKT_4] = {0x16, 0x01, 0x03, 0xE6};                                      // CM1106 response OK
static byte response[MB_PKT_8] = {0};
static byte responseval[MB_PKT_17] = {0};
static byte responseCM[MB_PKT_4] = {0};

bool isLongPress = false;
bool isLongPressBEEP = false;
bool ExitCali = false;

byte VALbeep = 0;
int CO2 = 0;
unsigned int ConnRetry = 0;
unsigned int crc_cmd;
unsigned int DelayVal = 0;
unsigned int CalibVal = 0;         // start reading from the first byte (address 0) of the EEPROM
int VALDIS = 0;
unsigned long LongPress_ms = 5000; // 5s button timeout
unsigned long StartPress_ms = 0;

union BYTE_FLOAT_CO2
{
  byte uByte[4];
  float uCO2;
} u;

//SevenSegmentExtended display(A4, A5);
SoftwareSerial co2sensor(PIN_RX, PIN_TX);
#ifdef SCD30
#define BAUDRATE 19200 // Device to SCD30 Serial baudrate (should not be changed)
#else
#define BAUDRATE 9600 // Device to MHZ19 and CM1106 Serial baudrate (should not be changed)
#endif
#ifdef MHZ14_9
MHZ19 co2MHZ;
#endif

void setup()
{
  Timer1.initialize();
  MFS.initialize(&Timer1); // initialize multi-function shield library
  Serial.begin(115200);

#ifdef SCD30
  Serial.println("Start SCD30 Modbus lecture");
  co2sensor.begin(BAUDRATE); // (Uno example) device to SCD30 serial start
#endif
#ifdef MHZ14_9
  Serial.println("Start MH-Z14 or MH-Z19 lecture");
  co2sensor.begin(BAUDRATE);
  co2MHZ.begin(co2sensor);
#endif
#ifdef CM1106
  Serial.println("Start CM1106 lecture");
  co2sensor.begin(BAUDRATE); // (Uno example) device to CM1106 serial start
#endif
  delay(1000);

  VALbeep = EEPROM.read(address);
  displayVALbeep();
  delay(1500);

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

  // Preheat routine: min 30 seconds for SCD30
  Serial.print("Preheat: ");
#ifdef MHZ14_9
  //  for (int i = 180; i > -1; i--)
  for (int i = 18; i > -1; i--)
#else
  for (int i = 30; i > -1; i--)
#endif
  { // Preheat from 0 to 30 or to 180
    MFS.write("HEAT");
    delay(1000);
    MFS.write(i);
    Serial.println(i);
    delay(1000);
    i--;
  }
  Serial.println("Start measurements");
  MFS.write("CO2-");
  delay(5000);
}

void loop()
{
#ifdef SCD30
  CO2 = co2SCD30();
#endif
#ifdef MHZ14_9
  CO2 = co2MHZ14_9();
#endif
#ifdef CM1106
  CO2 = co2CM1106();
#endif

  if (CO2 > 0)
  {
    Serial.print("CO2(ppm): ");
    Serial.println(CO2);

    MFS.write("");
    delay(10);
    MFS.write(CO2);
  }
  else
  {
    Serial.println("ERROR CO2=0");
    delay(10);
  }

  // Alarm if CO2 is greater than 1000

  if (CO2 >= VALDIS)
  {
    Beep();
    MFS.writeLeds(LED_ALL, ON);
    MFS.blinkLeds(LED_ALL, ON);
  }
  else {
    MFS.blinkLeds(LED_ALL, OFF);
    MFS.writeLeds(LED_ALL, OFF);
  }

  check_calmode_active();
  Serial.println("Waiting for new data");
  delay(2000);
}

// Initialice SCD30

#ifdef SCD30
void CO2iniSCD30()
{
  //Start continious measurement
  co2sensor.write(cmdConM, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.println("Start continious measurement");
  CheckResponse(cmdConM, response, MB_PKT_8);
  delay(1000);

  //Set measurement interval 2 seconds
  co2sensor.write(cmdSetM, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.println("Set measurement interval 2 seconds ");
  CheckResponse(cmdSetM, response, MB_PKT_8);
  delay(1000);

  //Deactivate Automatic Self-Calibration
  co2sensor.write(cmdAuto, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.println("Deactivate Automatic Self-Calibration ");
  CheckResponse(cmdAuto, response, MB_PKT_8);
  delay(1000);

  while (co2SCD30() == 0 && (ConnRetry < 5))
  {
    Serial.println("Air sensor not detected. Please check wiring... Try# " + String(ConnRetry));
    MFS.write("");
    delay(20);
    MFS.write("bad");
    delay(2500);
    ConnRetry++;
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println("Air sensor detected SCD30 Modbus");

  MFS.write("");
  delay(10);
  MFS.write("good");
  delay(2500);
  Serial.println("SCD30 read OK");
  delay(4000);
}
#endif

// Initialice MHZ19

#ifdef MHZ14_9
void CO2iniMHZ19()
{
  co2MHZ.autoCalibration(false); // Turn OFF calibration
  Serial.print("MHZ19 Stop Autocalibration: ");
  co2MHZ.getABC() ? Serial.println(" failed") : Serial.println(" done"); // now print it's status
  delay(100);

  while (co2MHZ.getCO2() == 0 && (ConnRetry < 5))
  {
    Serial.println("Air sensor not detected. Please check wiring... Try# " + String(ConnRetry));
    MFS.write("");
    delay(20);
    MFS.write("bad");
    delay(2500);
    ConnRetry++;
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println("Air sensor detected MHZ19");
  MFS.write("");
  delay(10);
  MFS.write("good");
  Serial.println("MHZ19 read OK");
  delay(4000);
}
#endif

// Initialize CM1106
#ifdef CM1106
void CO2iniCM1106()
{
  // Stop Autocalibration
  co2sensor.write(cmdOFFa, MB_PKT_10);
  co2sensor.readBytes(responseCM, MB_PKT_4);

  Serial.print("CM1106 Stop Autocalibration: ");
  CheckResponse(cmdOKqu, responseCM, MB_PKT_4);
  delay(1000);

  // Connection test
  while (co2CM1106() == 0 && (ConnRetry < 5))
  {
    Serial.println("Air sensor not detected. Please check wiring... Try# " + String(ConnRetry));
    MFS.write("");
    delay(20);
    MFS.write("bad");
    delay(2500);
    ConnRetry++;
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println("Air sensor detected CM1106");
  MFS.write("");
  delay(10);
  MFS.write("good");

  Serial.println("CM1106 read OK");
  delay(4000);
}
#endif

// CO2 lecture SCD30
#ifdef SCD30
int co2SCD30()
{
  memset(responseval, 0, MB_PKT_17);
  co2sensor.write(cmdRead, MB_PKT_8);
  co2sensor.readBytes(responseval, MB_PKT_17);
  Serial.print("Read Sensor: ");

  // CRC Routine
  u.uByte[0] = responseval[6];
  u.uByte[1] = responseval[5];
  u.uByte[2] = responseval[4];
  u.uByte[3] = responseval[3];

  if (u.uCO2 != 0)
  {
    crc_cmd = crcx::crc16(responseval, 15);
    if (responseval[15] == lowByte(crc_cmd) && responseval[16] == highByte(crc_cmd))
    {
      Serial.println(" OK");
      return (u.uCO2);
    }
    else
    {
      Serial.println("FAIL");
      MFS.write("fail");
      return (u.uCO2);
    }
  }
  else
  {
    Serial.println("FAIL");
    MFS.write("fail");
    return (u.uCO2);
  }
}
#endif

#ifdef MHZ14_9
//Read MHZ14 or 19
int co2MHZ14_9()
{
  CO2 = co2MHZ.getCO2();
  if (CO2 != 0)
  {
    return CO2;
  }
  else
  {
    Serial.println("failed");
    MFS.write("");
    delay(10);
    MFS.write("fail");
    return (CO2);
  }
}
#endif

//Read CM1106
#ifdef CM1106
int co2CM1106()
{
  memset(response, 0, MB_PKT_8);
  co2sensor.write(cmdReCM, MB_PKT_4);
  co2sensor.readBytes(response, MB_PKT_8);
  CO2 = (256 * response[3]) + response[4];
  if (CO2 != 0)
  {
    int crc = 0;
    for (int i = 0; i < 7; i++)
      crc += response[i];
    crc = 256 - crc % 256;
    if ((response[0] == 0x16) && (response[7] == crc))
    {
      return CO2;
    }
    else
    {
      while (co2sensor.available() > 0)
        char t = co2sensor.read(); // Clear serial input buffer;
      return 0;
    }
  }
  else
  {
    Serial.println("failed");
    MFS.write("");
    delay(10);
    MFS.write("fail");
    return (CO2);
  }
}
#endif

// Beep 900Hhz
void Beep()
{
  MFS.beep(33);
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
    MFS.write("");
    delay(10);
    MFS.write("done");
  }
  else
  {
    Serial.println("failed");
    MFS.write("");
    delay(10);
    MFS.write("fail");
  }
}

// Press button routine for Calibration

// First. put the sensor outside in a close box and wait 5 minutes, this is the reference of 400ppm
// Open and push the button more than 5 seconds
// Close the box and the sensor enter to the calibration process
// At the end the sensor receives the order of calibration to 400ppm

void check_calmode_active()
{
  unsigned long currentTime_ms = millis();

  // Test CALIB button

  if (digitalRead(BUTTON_CALIB) == LOW)
  {
    if (isLongPress)
    {
      if (currentTime_ms > (StartPress_ms + LongPress_ms))
      {
        ExitCali = false;
        MFS.write("");
        delay(100);
#ifdef MHZ14_9
        Serial.println("Start calibration process: 1200 seconds of 400 ppm stable");
        //        for (int i = 1200; i > -1; i--)
        for (int i = 12; i > -1; i--)
#else
        Serial.println("Start calibration process: 300 seconds of 400 ppm stable");
        for (int i = 20; i > -1; i--)
        //for (int i = 300; i > -1; i--)
#endif
        { // loop from 0 to 300
          MFS.write("CAL-");
          delay(1000);
          Serial.print(i);
          Serial.print(" ");

#ifdef SCD30
          CO2 = co2SCD30();
#endif
#ifdef MHZ14_9
          CO2 = co2MHZ.getCO2();
#endif
#ifdef CM1106
          CO2 = co2CM1106();
#endif

          Serial.print("CO2(ppm): ");
          Serial.println(CO2);
          MFS.write(i);
          delay(1000);
          i--;

          if (digitalRead(BUTTON_ENTER) == LOW) {
            delay(50);
            if (digitalRead(BUTTON_ENTER) == LOW) {
              i = 0;
              ExitCali = true;
              break;
            }
          }
        }

        if  (ExitCali == false) {
#ifdef SCD30
          CalibrationSCD30();
#endif
#ifdef MHZ14_9
          CalibrationMHZ19();
#endif
#ifdef CM1106
          CalibrationCM1106();
#endif
          isLongPress = false;
        }
        ExitCali = false;
      }
    }
    else
    {
      StartPress_ms = millis();
      isLongPress = true;
      Serial.println("Button CALIB has been pressed, hold 5s more to start calibration");
    }
  }
  else
  {
    isLongPress = false;
  }

  // Test BEEP button

  if (digitalRead(BUTTON_BEEP) == LOW)
  {
    if (isLongPressBEEP)
    {
      if (currentTime_ms > (StartPress_ms + LongPress_ms))
      {
        Serial.println("Routine BEEP");
        displayVALbeep();

        while (digitalRead(BUTTON_ENTER) == HIGH) {
          delay(10);

          if (digitalRead(BUTTON_BEEP) == LOW) {
            delay (50);
            if (digitalRead(BUTTON_BEEP) == LOW) {
              VALbeep++;
              if (VALbeep > 7 || VALbeep == 0xFF) VALbeep = 0;
              displayVALbeep();
              delay(200);
            }
          }
        }
        delay(50);
        if (digitalRead(BUTTON_ENTER) == LOW) {
          EEPROM.write(address, VALbeep);
          isLongPressBEEP = false;
          Serial.print("New Beep Value: ");
          Serial.println(VALDIS);
        }
        else {
          Serial.println("no change in BEEP value");
        }
      }

    }
    else
    {
      StartPress_ms = millis();
      isLongPressBEEP = true;
      Serial.println("Button BEEP has been pressed, hold 5s more to start BEEP level change routine");
    }
  }
  else
  {
    isLongPressBEEP = false;
  }
}

void displayVALbeep() {
  if (VALbeep == 0xFF) VALbeep = 0;
  VALDIS = 1000 + (VALbeep * 100);
  if (VALDIS > 1300) {
    VALDIS = VALDIS - 700;
  }
  Serial.print("BEEP level: ");
  Serial.println(VALDIS);
  MFS.write("BEEP");
  delay(1000);
  MFS.write(VALDIS);
}

// Calibration routine
#ifdef SCD30
void CalibrationSCD30()
{
  delay(100);

  //Calibration set to 400ppm
  co2sensor.write(cmdCali, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print("Resetting forced calibration factor to 400: ");
  CheckResponse(cmdCali, response, MB_PKT_8);
  delay(4000);
}
#endif

#ifdef MHZ14_9
void CalibrationMHZ19()
{
  delay(100);
  Serial.print("Resetting forced calibration factor to 400: ");
  co2MHZ.calibrate(); // Take a reading which be used as the zero point for 400 ppm
  Serial.println("done");
  MFS.write("done");
  delay(4000);
}
#endif

#ifdef CM1106
void CalibrationCM1106()
{
  delay(100);
  Serial.print("Resetting forced calibration factor to 400: ");
  co2sensor.write(cmdCalC, MB_PKT_6);
  co2sensor.readBytes(responseCM, MB_PKT_4);
  CheckResponse(cmdOKca, responseCM, MB_PKT_4);
  delay(4000);
}
#endif

//Software RESET

void softwareReset(uint8_t prescaller)
{
  delay(2000);
  Serial.println("RESET...");
  wdt_enable(prescaller);
  while (1)
  {
  }
}
