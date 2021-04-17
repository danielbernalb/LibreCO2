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
  Botón de Calibración o cable conectado a pin 4
  Botón de cambio del nivel del BEEP o chicharra conectado a pin A3

  Rev 142
*/

// Uncomment your CO2 sensor

//#define SCD30           // Sensirion SCD30
//#define MHZ14_9       // Winsen MHZ14 or 19
//#define CM1106        // Cubic CM1106
#define SenseAir_S8 // SenseAir S8

#include <CRCx.h> //https://github.com/hideakitai/CRCx
#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <MultiFuncShield.h>
#include "MHZ19.h" //https://github.com/WifWaf/MH-Z19
#include <EEPROM.h>
#include <avr/wdt.h>

const byte RevVersion = 142; // Firmware version 16 april 2021

const byte PIN_TX = 5;  // define TX pin to Sensor
const byte PIN_RX = 6;  // define RX pin to Sensor
const byte PIN_SEL = 9; // define SEL pin for SCD30
const byte BUTTON_CALIB = A1;
const byte BUTTON_BEEP = A2;
const byte BUTTON_ENTER = A3;
const byte address = 0;    // EEPROM address 0

const byte MB_PKT_4 = 4;   //CM1106 receive Packet Size
const byte MB_PKT_6 = 6;   //CM1106 receive Packet Size
const byte MB_PKT_7 = 7;   //MODBUS Packet Size
const byte MB_PKT_8 = 8;   //MODBUS Packet Size
const byte MB_PKT_10 = 10; //CM1106 send Packet Size
const byte MB_PKT_17 = 17; // MODBUS Packet Size

const byte cmdConM[MB_PKT_8] = {0x61, 0x06, 0x00, 0x36, 0x00, 0x00, 0x60, 0x64}; // SCD30 Trigger continuous measurement with no ambient pressure compensation
const byte cmdSetM[MB_PKT_8] = {0x61, 0x06, 0x00, 0x25, 0x00, 0x02, 0x10, 0x60}; // SCD30 Set measurement interval 2 seconds
const byte cmdAuto[MB_PKT_8] = {0x61, 0x06, 0x00, 0x3A, 0x00, 0x00, 0xA0, 0x67}; // SCD30 Deactivate Automatic Self-Calibration
const byte cmdRead[MB_PKT_8] = {0x61, 0x03, 0x00, 0x28, 0x00, 0x06, 0x4C, 0x60}; // SCD30 Read CO2
const byte cmdCali[MB_PKT_8] = {0x61, 0x06, 0x00, 0x39, 0x01, 0x90, 0x51, 0x9B}; // SCD30 Calibrate to 400pm

const byte cmdReCM[MB_PKT_4] = {0x11, 0x01, 0x01, 0xED};                                      // CM1106 Read CO2
const byte cmdOFFa[MB_PKT_10] = {0x11, 0x07, 0x10, 0x64, 0x02, 0x07, 0x01, 0x90, 0x64, 0x76}; // CM1106 Command Close ABC
const byte cmdCalC[MB_PKT_6] = {0x11, 0x03, 0x03, 0x01, 0x90, 0x58};                          // CM1106 Command Calibration of CO2 Concentration to 400ppm
const byte cmdOKqu[MB_PKT_4] = {0x16, 0x01, 0x10, 0xD9};                                      // CM1106 response OK
const byte cmdOKca[MB_PKT_4] = {0x16, 0x01, 0x03, 0xE6};                                      // CM1106 response OK

const byte cmdReSA[MB_PKT_8] = {0xFE, 0X04, 0X00, 0X03, 0X00, 0X01, 0XD5, 0XC5}; // SenseAir Read CO2
const byte cmdOFFs[MB_PKT_8] = {0xFE, 0x06, 0x00, 0x1F, 0x00, 0x00, 0xAC, 0x03}; // SenseAir Close ABC
const byte cmdCal1[MB_PKT_8] = {0xFE, 0x06, 0x00, 0x00, 0x00, 0x00, 0x9D, 0xC5}; // SenseAir Calibration 1
const byte cmdCal2[MB_PKT_8] = {0xFE, 0x06, 0x00, 0x01, 0x7C, 0x06, 0x6C, 0xC7}; // SenseAir Calibration 2
const byte cmdCal3[MB_PKT_8] = {0xFE, 0x03, 0x00, 0x00, 0x00, 0x01, 0x90, 0x05}; // SenseAir Calibration 3
const byte cmdCalR[MB_PKT_7] = {0xFE, 0x03, 0x02, 0x00, 0x20, 0xAD, 0x88};       // SenseAir Calibration Response

static byte response[MB_PKT_8] = {0};
static byte responseSA[MB_PKT_7] = {0};
static byte responseval[MB_PKT_17] = {0};
static byte responseCM[MB_PKT_4] = {0};

bool isLongPress = false;
bool isLongPressBEEP = false;
bool ExitCali = false;

byte VALbeep = 0;
int CO2 = 0;
int CO2temp = 0;
unsigned int ConnRetry = 0;
unsigned int crc_cmd;
unsigned int DelayVal = 0;
unsigned int CalibVal = 0; // start reading from the first byte (address 0) of the EEPROM
int VALDIS = 0;
unsigned long LongPress_ms = 5000; // 5s button timeout
unsigned long StartPress_ms = 0;

union BYTE_FLOAT_CO2
{
  byte uByte[4];
  float uCO2;
} u;

SoftwareSerial co2sensor(PIN_RX, PIN_TX);
#ifdef SCD30
#define BAUDRATE 19200 // Device to SCD30 Serial baudrate (should not be changed)
#else
#define BAUDRATE 9600 // Device to SenseAir S8, MHZ19 and CM1106 Serial baudrate (should not be changed)
#endif
#ifdef MHZ14_9
MHZ19 co2MHZ;
#endif

void setup()
{
  Timer1.initialize();
  MFS.initialize(&Timer1); // initialize multi-function shield library
  Serial.begin(115200);

  Serial.print("Firmware version: ");
  Serial.println(RevVersion);

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
#ifdef SenseAir_S8
  Serial.println("Start SenseAir S8 lecture");
  co2sensor.begin(BAUDRATE);
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
#ifdef SenseAir_S8
  CO2iniSenseAir();
#endif

  // Preheat routine: min 30 seconds for SCD30 & CM1106 & SenseAir S8, 180 seconds for MH-Z14 or 19
  Serial.print("Preheat: ");
#ifdef MHZ14_9
  for (int i = 180; i > -1; i--)
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
#ifdef SenseAir_S8
  CO2 = co2SenseAir();
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

  // Alarm if CO2 is greater than VALDIS

  if (CO2 >= VALDIS && VALDIS != 1450)
  {
    Beep();
    MFS.writeLeds(LED_ALL, ON);
    MFS.blinkLeds(LED_ALL, ON);
  }
  else
  {
    MFS.blinkLeds(LED_ALL, OFF);
    MFS.writeLeds(LED_ALL, OFF);
  }

  check_calmode_active();
#ifdef SCD30
  delay(1971); // for SCD30 2 seconds
#endif
#ifdef MHZ14_9
  delay(1971); // for SCD30 2 seconds
#endif
#ifdef CM1106
  delay(1971); // for SCD30 2 seconds
#endif
#ifdef SenseAir_S8
  delay(3967); // for SenseAir S8LP 4 seconds
#endif
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

  Serial.println("Deactivate Automatic Self-Calibration SCD30");
  CheckResponse(cmdAuto, response, MB_PKT_8);
  delay(2000);

  while (co2SCD30() == 0 && (ConnRetry < 5))
  {
    BadConn();
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
  Serial.print("MHZ14_9 Stop Autocalibration: ");
  co2MHZ.getABC() ? Serial.println(" failed") : Serial.println(" done"); // now print it's status
  delay(100);

  while (co2MHZ.getCO2() == 0 && (ConnRetry < 5))
  {
    BadConn();
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println("Air sensor detected MHZ14_9");
  MFS.write("");
  delay(10);
  MFS.write("good");
  Serial.println("MHZ14_9 read OK");
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
    BadConn();
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

// Initialice SCD30

#ifdef SenseAir_S8
void CO2iniSenseAir()
{
  //Deactivate Automatic Self-Calibration
  co2sensor.write(cmdOFFs, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.println("Deactivate Automatic Self-Calibration SenseAir");
  CheckResponse(cmdOFFs, response, MB_PKT_8);
  delay(2000);

  while (co2SenseAir() == 0 && (ConnRetry < 5))
  {
    BadConn();
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println("Air sensor detected AirSense S8 Modbus");

  MFS.write("");
  delay(10);
  MFS.write("good");
  delay(2500);
  Serial.println("SenseAir read OK");
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
  ////
  Serial.print("Read SCD30: ");

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
      Serial.print("FAIL CO2 old: ");
      Serial.println(CO2);
      CO2temp = u.uCO2;
      Serial.print("FAIL CRC_CO2actual: ");
      Serial.println(CO2temp);

      if (CO2temp - CO2 < 10 && CO2temp - CO2 > -10)
      {
        return (u.uCO2);
      }
      else
      {
        if (CO2 == 0)
        {
          Serial.println("FAIL CRC_CO2 = 0");
          MFS.write("----");
          return 0;
        }
        else
        {
          Serial.println("FAIL CRC CO2 last");
          return CO2;
        }
      }
    }
  }

  else
  {
    if (CO2 == 0)
    {
      Serial.println("FAIL CRC_CO2 = 0");
      MFS.write("----");
      return 0;
    }
    else
    {
      Serial.println("FAIL CRC CO2 continuously");
      //return CO2;
      MFS.write("----");
      return 0;
    }
  }
}
#endif

#ifdef MHZ14_9
//Read MHZ14 or 19
int co2MHZ14_9()
{
  CO2 = co2MHZ.getCO2();

  Serial.print("Read MHZ14_9: ");

  if (CO2 != 0)
  {
    Serial.println("OK DATA");
    return CO2;
  }
  else
  {
    Serial.println("FAILED");
    MFS.write("");
    delay(10);
    MFS.write("----");
    return 0;
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

  Serial.print("Read CM1106: ");

  if (CO2 != 0)
  {

    int crc = 0;
    for (int i = 0; i < 7; i++)
      crc += response[i];
    crc = 256 - crc % 256;
    if ((response[0] == 0x16) && (response[7] == crc))
    {
      Serial.println("OK DATA");
      return CO2;
    }
    else
    {
      while (co2sensor.available() > 0)
        char t = co2sensor.read(); // Clear serial input buffer;

      Serial.print("FAIL CRC_CO2");
      MFS.write("----");
      return 0;
    }
  }
  else
  {
    Serial.println("FAILED");
    MFS.write("");
    delay(10);
    MFS.write("----");
    return 0;
  }
}
#endif

// CO2 lecture SenseAir S8
#ifdef SenseAir_S8
int co2SenseAir()
{
  memset(responseSA, 0, MB_PKT_7);
  co2sensor.write(cmdReSA, MB_PKT_8);
  co2sensor.readBytes(responseSA, MB_PKT_7);
  CO2 = (256 * responseSA[3]) + responseSA[4];

  Serial.print("Read SenseAir S8: ");

  if (CO2 != 0)
  {

    crc_cmd = crcx::crc16(responseSA, 5);
    if (responseSA[5] == lowByte(crc_cmd) && responseSA[6] == highByte(crc_cmd))
    {
      Serial.println("OK DATA");
      return CO2;
    }
    else
    {
      while (co2sensor.available() > 0)
        char t = co2sensor.read(); // Clear serial input buffer;

      Serial.print("FAIL CRC_CO2");
      MFS.write("----");
      return 0;
    }
  }
  else
  {
    Serial.println("FAILED");
    MFS.write("");
    delay(10);
    MFS.write("----");
    return 0;
  }
}
#endif

//Routine Bad connection

void BadConn()
{
  Serial.println("Air sensor not detected. Please check wiring... Try# " + String(ConnRetry));
  MFS.write("");
  delay(20);
  MFS.write("bad");
  delay(2500);
  ConnRetry++;
}

// Beep 900Hhz
void Beep()
{
  if (VALbeep != 9)
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
        for (int i = 1200; i > -1; i--)
#else
        Serial.println("Start calibration process: 300 seconds of 400 ppm stable");
        for (int i = 300; i > -1; i--)
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
#ifdef SenseAir_S8
          CO2 = co2SenseAir();
#endif
          Serial.print("CO2(ppm): ");
          Serial.println(CO2);
          MFS.write(i);
          delay(1000);
          i--;

          if (digitalRead(BUTTON_ENTER) == LOW)
          {
            delay(50);
            if (digitalRead(BUTTON_ENTER) == LOW)
            {
              i = 0;
              Serial.println("Calibration process canceled");
              ExitCali = true;
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

        while (digitalRead(BUTTON_ENTER) == HIGH)
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
              displayVALbeep();
              delay(500);
            }
          }
        }
        delay(50);
        if (digitalRead(BUTTON_ENTER) == LOW)
        {
          EEPROM.write(address, VALbeep);
          isLongPressBEEP = false;
          Serial.print("New Beep Value: ");
          if (VALDIS == 1450)
            Serial.println("OFF");
          else
            Serial.println(VALDIS);
        }
        else
        {
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

void displayVALbeep()
{
  if (VALbeep == 0xFF)
    VALbeep = 0;
  VALDIS = 1000 + (VALbeep * 50);
  if (VALDIS > 1450)
  {
    VALDIS = VALDIS - 800;
  }
  Serial.print("BEEP level: ");
  MFS.write("BEEP");
  delay(1000);
  if (VALDIS == 1450)
  {
    MFS.write("OFF");
    Serial.println("OFF");
  }
  else
  {
    MFS.write(VALDIS);
    Serial.println(VALDIS);
  }
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
  //Calibration set to 400ppm
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

  //Calibration set to 400ppm
  co2sensor.write(cmdCalC, MB_PKT_6);
  co2sensor.readBytes(responseCM, MB_PKT_4);

  Serial.print("Resetting forced calibration factor to 400: ");
  CheckResponse(cmdOKca, responseCM, MB_PKT_4);
  delay(4000);
}
#endif

#ifdef SenseAir_S8
void CalibrationSenseAir()
{
  delay(100);

  //Step 1 Calibration
  co2sensor.write(cmdCal1, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print("Calibration Step1: ");
  CheckResponse(cmdCal1, response, MB_PKT_8);
  delay(1000);

  //Step 2 Calibration
  co2sensor.write(cmdCal2, MB_PKT_8);
  co2sensor.readBytes(response, MB_PKT_8);

  Serial.print("Calibration Step2: ");
  CheckResponse(cmdCal2, response, MB_PKT_8);
  delay(4000);

  //Step 3 Calibration
  co2sensor.write(cmdCal3, MB_PKT_8);
  co2sensor.readBytes(responseSA, MB_PKT_7);

  Serial.print("Resetting forced calibration factor to 400: ");
  CheckResponse(cmdCalR, responseSA, MB_PKT_7);
  delay(3000);
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
