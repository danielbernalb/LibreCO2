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

// Quita al comentario respecto al sensor de CO2 que usas
//#define SCD30
//#define MHZ14_9       // uncomment on the supported CO2 sensors
#define CM1106

#include <CRCx.h> //https://github.com/hideakitai/CRCx, install it from Library Manager as CRCx
#include <SoftwareSerial.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include "MHZ19.h"
#include <avr/wdt.h>

const byte PIN_TX = 6;  // define CLK pin (any digital pin)
const byte PIN_RX = 7;  // define DIO pin (any digital pin)
const byte PIN_CLK = 9; // define CLK pin (any digital pin)
const byte PIN_DIO = 8; // define DIO pin (any digital pin)
const byte BUTTON = 2;  // define DIO pin (any digital pin)
const byte BUZZER = 11;
const int MB_PKT_8 = 8; //MODBUS Packet Size
const int MB_PKT_17 = 17; // MODBUS Packet Size
const int MB_PKT_10 = 10; //CM1106 send Packet Size
const int MB_PKT_6 = 6; //CM1106 receive Packet Size
const int MB_PKT_4 = 4; //CM1106 receive Packet Size

unsigned int CO2 = 0;
unsigned int ConnRetry = 0;
uint16_t crc_cmd;
unsigned int DelayVal = 0;
unsigned int CalibVal = 0;
union BYTE_FLOAT_CO2
{
  byte uByte[4];
  float uCO2;
} u;

static byte response[MB_PKT_8] = {0};
static byte responseval[MB_PKT_17] = {0};
static byte responseCM[MB_PKT_4] = {0};
const byte cmdConM[MB_PKT_8] = {0x61, 0x06, 0x00, 0x36, 0x00, 0x00, 0x60, 0x64}; // SCD30 Trigger continuous measurement with no ambient pressure compensation
const byte cmdSetM[MB_PKT_8] = {0x61, 0x06, 0x00, 0x25, 0x00, 0x02, 0x10, 0x60}; // SCD30 Set measurement interval 2 seconds
const byte cmdAuto[MB_PKT_8] = {0x61, 0x06, 0x00, 0x3A, 0x00, 0x00, 0xA0, 0x67}; // SCD30 Deactivate Automatic Self-Calibration
//const byte cmdRaut[MB_PKT_8] = {0x61, 0x03, 0x00, 0x3A, 0x00, 0x01, 0xAD, 0xA7}; // SCD30 Request Auto calibration status
const byte cmdRead[MB_PKT_8] = {0x61, 0x03, 0x00, 0x28, 0x00, 0x06, 0x4C, 0x60}; // SCD30 Read CO2
const byte cmdCali[MB_PKT_8] = {0x61, 0x06, 0x00, 0x39, 0x01, 0x90, 0x51, 0x9B}; // SCD30 Calibrate to 400pm
const byte cmdReCM[MB_PKT_4] = {0x11, 0x01, 0x01, 0xED}; // CM1106 Read
const byte cmdOFFa[MB_PKT_10] = {0x11, 0x07, 0x10, 0x64, 0x02, 0x07, 0x01, 0x90, 0x64, 0x76}; // CM1106 Command Close ABC
const byte cmdCalC[MB_PKT_6] = {0x11, 0x03, 0x03, 0x01, 0x90, 0x58}; // CM1106 Command Calibration of CO2 Concentration to 400ppm
const byte cmdOKqu[MB_PKT_4] = {0x16, 0x01, 0x10, 0xD9}; // CM1106 response OK
const byte cmdOKca[MB_PKT_4] = {0x16, 0x01, 0x03, 0xE6}; // CM1106 response OK

unsigned long LongPress_ms = 5000; // 5s button timeout
unsigned long StartPress_ms = 0;
bool isLongPress = false;

#ifdef SCD30
#define BAUDRATE 19200 // Device to SCD30 Serial baudrate (should not be changed)
#else
#define BAUDRATE 9600 // Device to MHZ19 and CM1106 Serial baudrate (should not be changed)
#endif

SevenSegmentExtended display(PIN_CLK, PIN_DIO);

#ifdef MHZ14_9
SoftwareSerial mySerial(PIN_RX, PIN_TX);
MHZ19 co2MHZ;
#else
SoftwareSerial co2sensor(PIN_RX, PIN_TX);
#endif

void setup()
{
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  Serial.begin(115200);
#ifdef SCD30
  Serial.println("Start SCD30 Modbus lecture");
  co2sensor.begin(BAUDRATE); // (Uno example) device to SCD30 serial start
#endif
#ifdef MHZ14_9
  Serial.println("Start MH-Z14 or MH-Z19 lecture");
  mySerial.begin(BAUDRATE);
  co2MHZ.begin(mySerial);
#endif
#ifdef CM1106
  Serial.println("Start CM1106 lecture");
  co2sensor.begin(BAUDRATE); // (Uno example) device to CM1106 serial start
#endif
  display.begin();           // initializes the display
  display.setBacklight(100); // set the brightness to 100 %
  delay(1000);

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
  display.clear();
  Serial.print("Preheat: ");
#ifdef MHZ14_9
  for (int i = 180; i > -1; i--)
//  for (int i = 18; i > -1; i--)
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
  Serial.println("Star measurements");
  display.clear();
  display.print("CO2-");
  delay(5000);
}

void loop()
{
#ifdef SCD30
  CO2 = co2SCD30();
#endif
#ifdef MHZ14_9
  CO2 = co2MHZ.getCO2();
#endif
#ifdef CM1106
  CO2 = co2CM1106();
#endif

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
    Serial.println("ERROR CO2=0");
    delay(10);
  }

  // Alarm if CO2 is greater than 1000

  if (CO2 > 1000)
  {
    Beep();
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
    display.clear();
    delay(20);
    display.print("bad");
    delay(2500);
    ConnRetry++;
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println("Air sensor detected SCD30 Modbus");
  display.clear();
  delay(10);
  display.print("good");
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
    display.clear();
    delay(20);
    display.print("bad");
    delay(2500);
    ConnRetry++;
  }

  if (ConnRetry == 5)
    softwareReset(WDTO_60MS);

  Serial.println("Air sensor detected MHZ19");
  display.clear();
  delay(10);
  display.print("good");
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
  delay(100);

  // Connection test
  while (co2CM1106() == 0 && (ConnRetry < 5))
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

  Serial.println("Air sensor detected CM1106");
  display.clear();
  delay(10);
  display.print("good");

  Serial.println("CM1106 read OK");
  delay(4000);

}
#endif

// CO2 lecture SCD30
#ifdef SCD30
int co2SCD30()
{
  responseval[MB_PKT_17] = {0};
  co2sensor.write(cmdRead, MB_PKT_8);
  co2sensor.readBytes(responseval, MB_PKT_17);
  Serial.print("Read Sensor: ");

  // CRC Routine
  u.uByte[0] = responseval[6];
  u.uByte[1] = responseval[5];
  u.uByte[2] = responseval[4];
  u.uByte[3] = responseval[3];
  //     Serial.print(" num0 ");
  //    Serial.print(u.uCO2);

  if (u.uCO2 != 0) {
    crc_cmd = crcx::crc16(responseval, 15);
    if (responseval[15] == lowByte(crc_cmd) && responseval[16] == highByte(crc_cmd))
    {
      Serial.println(" OK");
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
      return (u.uCO2);
    }
  }
  else
  {
    Serial.println("FAIL");
    display.clear();
    display.print("fail");
    return (u.uCO2);
  }
}
#endif

//Read CM1106
#ifdef CM1106
int co2CM1106() {
  co2sensor.write(cmdReCM, MB_PKT_4);
  co2sensor.readBytes(response, MB_PKT_8);
  int crc = 0;
  for (int i = 0; i < 7; i++) crc += response[i];
  crc = 256 - crc % 256;
  if (((int) response[0] == 0x16) && ((int)response[7] == crc)) {
    unsigned int responseHigh = (unsigned int) response[3];
    unsigned int responseLow = (unsigned int) response[4];
    return (256 * responseHigh) + responseLow;
  } else {
    while (co2sensor.available() > 0)  char t = co2sensor.read(); // Clear serial input buffer;
    return 0;
  }
}
#endif

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
    delay(10);
    display.clear();
    display.print("done");
  }
  else
  {
    Serial.println("failed");
    display.clear();
    delay(10);
    display.print("fail");
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

  if (digitalRead(BUTTON) == LOW)
  {
    if (isLongPress)
    {
      if (currentTime_ms > (StartPress_ms + LongPress_ms))
      {
        display.clear();
        delay(100);
#ifdef MHZ14_9
        Serial.println("Start calibration process: 1200 seconds of 400 ppm stable");
        for (int i = 1200; i > -1; i--)
//        for (int i = 12; i > -1; i--)
#else
        Serial.println("Start calibration process: 300 seconds of 400 ppm stable");
//        for (int i = 300; i > -1; i--)
        for (int i = 30; i > -1; i--)
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
          CO2 = co2MHZ.getCO2();
#endif
#ifdef CM1106
          CO2 = co2CM1106();
#endif

          Serial.print("CO2(ppm): ");
          Serial.println(CO2);
          display.clear();
          display.printNumber(i);
          delay(1000);
          i--;
        }
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
  co2MHZ.calibrate();    // Take a reading which be used as the zero point for 400 ppm
  Serial.println("done");
  display.clear();
  display.print("done");
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