# LibreCO2
LibreCO2: CO2 meter using Arduino UNO and CO2 low cost sensor (Sensirion SCD30, Winsen MH-Z14 or MHZ-19 and Cubic CM1106).

  This is a simple version of a low cost CO2 meter with the most common materials on the market: an Arduino UNO and the three most popular low cost REAL CO2 sensors: Sensirion SCD30, Winsen MH-Z14 or 16 and Cubic CM1106. LibreCO2 uses an Arduino UNO but can be extend to NANO or MEGA, that are very popular in schools and frequently used in the technology and the electronics classes and the majority of schools have many of them. The code is the simplest possible and the .hex file is published for the programming of the Arduino with the software Xloader and you don't have to install Arduino software for compile the code. If your are looking for Wifi or Bluetooth connectivity you can find many open sources alternatives in a list at the end of the guide like [CanAirIO](https://github.com/kike-canaries/canairio_firmware). The CO2 meter uses the most populars components on the market, not the most advance, and the simplest building without the need of iron solder.

All sensors used are NDIR that is the actual standard for real CO2 measurements:

https://www.co2meter.com/blogs/news/6010192-how-does-an-ndir-co2-sensor-work

**Materials:**

There are two options for the assembly of the sensor, the simplest is using an Arduino Multifunction Shield (Shield) which already includes the 4-digit display, the buzzer and the buttons, which makes the assembly much easier. The second by assembling the parts individually.

# 1. Multifunction Shield option:

![Materials](https://github.com/danielbernalb/LibreCO2/blob/main/images/Materiales%20sensor.jpg)

1. Arduino UNO original or chinese version, the difference is the driver installation, both are very easy to find in any country and work well. Original at right and clone at left:

![Arduino original & clone](https://github.com/danielbernalb/LibreCO2/blob/main/images/arduino-uno-original-clone.jpg)

2. Arduino UNO Multifunction Shield.

3. 4 jumper cables female female.

4. Sensor, options:

	a. Sensirion SCD30, the most expensive (52 dollars) but in our test the best performance. Sensirion has distributors in the USA and Europe. Example:
	https://www.mouser.com/ProductDetail/Sensirion/SCD30/?qs=rrS6PyfT74fdywu4FxpYjQ%3D%3D
	
	![SCD30 Sensirion](https://github.com/danielbernalb/LibreCO2/blob/main/images/Sensirion%20SCD30.jpg)
	
	Pros: excellent performance (fast, reliable), fast shipping from USA or Europe.
	
	Cons: price (52 dollars). 
	
	For the connection between the Arduino and the sensor it uses the Modbus protocol that allow the direct conecction between the Arduino and the sensor withput the use of data driver. You must connect the SEL pin with the VIN pin of the sensor for activate the Modbus protocol.
	
	b. Winsen MH-Z14 or 19, one of the cheapest and most popular, good performance, some slow. Take care with fake clones!!!

	![Winsen MH-Z14a](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z14A.jpg)
	
	Pros: acceptable performance (slow compared with Sensirion, medium reliable), low price (18 dollars from China)
	
	Cons: slow shipping with aliexpress standard shipping (20-one month), fake copies (you have to be careful, recommended only buy from official store).

	Only buy MH-Z19 from the Winsen Supplier in Aliexpress, the market is invaded with fake copies, more info in: https://youtu.be/5_QQe75-SZI or in spanish https://emariete.com/sensores-co2-mh-z19b-falsos/

	![Original Winsen MH-Z19b](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z19B.jpg)

	Originals in Aliexpress:
	https://es.aliexpress.com/item/1005001865093513.html

	c. Cubic CM1106, the last option because the distribution in China is for second hand units and sometimes is not available.
	https://www.aliexpress.com/item/4001082699057.html
	
	![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Cubic%20CM1106.jpg)
	

****************************

**Connections for all sensors:**

**Sensirion SCD30**

PIN del Escudo ---> Sensor

GND ---> GND

+5 &nbsp;  ---> VIN

5 &nbsp; &nbsp; ---> RX/SDA

6 &nbsp; &nbsp; ---> TX/SCL

![SCD30 connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20Shield%20SCD30%20connection.jpg)


****************************
**Winsen MH-Z19B sensor**

GND ---> GND

+5 &nbsp; ---> VIN

5 &nbsp; &nbsp; ---> RX

6 &nbsp; &nbsp; ---> TX

![MH-Z19B connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20Shield%20MHZ19%20connection.jpg)


****************************
**Winsen MH-Z14A sensor**

PIN del Escudo ---> Sensor

GND ---> Pin 5 or 16 del conector

+5 &nbsp; ---> Pin 4 o 17 del conector

5 &nbsp; &nbsp; ---> Pin 3 o 18 del conector

6 &nbsp; &nbsp; ---> Pin 2 o 19 del conector

![MH-Z14A connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20Shield%20MHZ14%20connection.jpg)	


****************************
**Cubic CM1106 sensor**

PIN del Escudo ---> Sensor

GND ---> G Pin 2 de la regleta de 4 pines

+5 &nbsp; ---> V1 Pin 1 de la regleta de 4 pines

5 &nbsp; &nbsp; ---> R Pin 2 de la regleta de 5 pines

6 &nbsp; &nbsp; ---> T Pin 3 de la regleta de 5 pines

![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20Shield%20Cubic%20connection.jpg)

## Assembly video
(in spanish with subs in english):

[![](http://img.youtube.com/vi/KYHC06xhUu4/0.jpg)](http://www.youtube.com/watch?v=KYHC06xhUu4 "VideoLibreCO2")

# 2. Individual parts option:

1. Arduino UNO original or chinese.
2. Display TM1687
3. Jumper female - male
4.One or two buttons switch or cables (one for the calibration, another for the Beep level).
5. Optional: Buzzer

![Materials](https://github.com/danielbernalb/LibreCO2/blob/main/images/Materials%20all%20text.jpg)

6. Sensor, same options as multifunction option.

****************************

**Connections for all sensors:**

Electronic component ---> Arduino PIN

**Display---> Arduino PIN**

CLK    ---> 9

DIO    ---> 8

VCC    ---> IOREF in original or 5V in clone

GND    ---> GND


**Buzzer**

"+"    ---> 11

other  ---> GND


**Button Calibration**

Anyone ---> 2

Anyone ---> 4


**Optional Button Beep Level**

Anyone ---> A5

Anyone ---> A3



****************************
**Sensirion SCD30 sensor**

VIN    ---> 3.3V 

GND    ---> GND

TX/SCL ---> 7

RX/SDA ---> 6

SEL    ---> 3.3V


![SCD30 connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20SCD30%20connection%20Modbus.jpg)


****************************
**Winsen MH-Z19B sensor**

VIN Pin 1 of connector 4 pins ---> 5V 

GND Pin 2 of connector 4 pins ---> GND

RX Pin 2 of connector 5 pins ---> 6

TX Pin 3 of connector 5 pins ---> 7 


![MH-Z19B connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20MHZ19%20connection.jpg)


****************************
**Winsen MH-Z14A sensor**

Connector pin 2 or 19 pin ---> 6

Connector pin 3 or 18 pin ---> 7

Connector pin 4 or 17 pin ---> 5V

Connector pin 5 or 16 pin ---> GND

![MH-Z14A connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20MHZ14%20connection.jpg)	


****************************
**Cubic CM1106 sensor**

V1 Pin 1 of connector 4 pins ---> 5V 

G Pin 2 of connector 4 pins ---> GND

R Pin 2 of connector 5 pins ---> 6

T Pin 3 of connector 5 pins ---> 7 

![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20%20CM1106%20connection.jpg)

****************************
# 3. Burn the Firmware (code in the Arduino).

1. Connect the Arduino to your computer. If the driver is not automatically detected you have to identify if your Arduino is original or chinese clone:

1.1. If the Arduino is original, install Arduino with drivers: https://www.arduino.cc/en/Guide/ArduinoUno

1.2. If the Arduino is chinese clone please follow the instructions in this web page: https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers

2. For the You have two options:

2.1. The easy way if you don't want to install Arduino, load and compile the code, only you need to programming, use Xloader:

![Xloader](https://github.com/danielbernalb/LibreCO2/blob/main/images/Xloader1.jpg)

The left screenshot shows the window that Xloader displays when it starts.
Explora hasta encontrar el achivo .hex que vas a cargar en el Arduino. Cada versión de sensor (Sensirion, Winsen o Cubic) se encuentra disponible en el directorio "hex files". Ejemplo: "LibreCO2_Winsen_MHZ14_9.hex" es el archivo para el MH-Z14 o MH-Z19. No lo bajes directamente de la página con la opción "Guardar como", eso arruina el archivo. Usa los siguientes links para bajarlos y guarda el archivo como .hex (no como .txt):
 
 **Para la opción con Escudo Multifunción:
 
 
 
 **Para la opción por partes:**
 Sensirion: https://raw.githubusercontent.com/danielbernalb/LibreCO2/main/hex%20files/LibreCO2_Sensirion_SCD30.hex
 Winsen: https://raw.githubusercontent.com/danielbernalb/LibreCO2/main/hex%20files/LibreCO2_Winsen_MHZ14_9.hex
 Cubic: https://raw.githubusercontent.com/danielbernalb/LibreCO2/main/hex%20files/LibreCO2_Cubic_CM1106.hex

2.1.2. Select the device type you are going to load into (Uno/ATmega328).

2.1.3. Select the COM port where your Arduino is attached.

2.1.4. The baud rate should be 115200.

2.1.5. Press the “Upload” button.

2.1.6. When the .hex file has been loaded into the Arduino the right screenshot will show the size of the file uploaded, or an error message if the upload fails.


2.2. If you want to edit and compile the code please install the Arduino software in Windows, Mac or Linux:
  
  https://www.arduino.cc/en/software
  
  The following guide explains how to install Arduino on Windows, Mac or Linux and is very complete:
  https://learn.sparkfun.com/tutorials/installing-arduino-ide/all  
  
  On YouTube there are hundreds of tutorials to install Arduino on different operating systems and learn their programming, you can also collaborate by improving this code or create your own version.
  
  # 4. Sensor operation.
 
 When the Arduino programming has been done, we have several sensor operating modes and options:
 
 1. Normal mode: When the sensor is turned on, the BEEP warning appears and a number between 700 and 1400 shows the level of the alarm that we have programmed. In number 3 it will be explained how to modify it. After that the meter verifies the connection to the low cost sensor. If the "good" message appears, the connection is well made and the sensor heating process ("heat") will continue, which lasts 30 seconds for the Sensirion and Cubic sensors, and 3 minutes for the Winsen sensors. If the message is "fail" the connection has failed and we must verify the connection between the Arduino and the sensor. After heating the CO2 value in parts per million, ppm, appears on the screen. When the value is higher than the Beep level, the alarm sounds and the shield LEDs flash.

 2. Calibration Mode: [Explained in the attached video] (#assembly-video).
Sensor calibration is recommended after sensor assembly and when widely differing measurements are observed outdoors in the approximate 400 ppm range. All these low-cost sensors are calibrated outdoors, placing them in an outdoor space for a few minutes without receiving strong wind as it affects the measurement, so it is recommended to place them inside a box that avoids direct wind.
After fulfilling these conditions, you must press the S1-A1 button in the version with shield, or the "switch" in the version by parts, for more than 5 seconds, and the "cal-" will appear on the screen, counting down 300 seconds for the Sensirion and Cubic and 20 minutes for the Winsen.
At the end of this time, the sensor receives the calibration order and is ready to be used again. If you want to interrupt the calibration process, press the S3-A3 button.

3. BEEP alarm threshold level change mode: [Explained in the attached video] (#assembly-video).
To change the alarm level from 1000 ppm, by default, to another value between 700 and 1400 ppm you must press button S2-A2 in the version with shield, or place an additional cable or switch from pin A3 to A5 in the version in parts, for more than 5 seconds and the message "BEEP" will appear and then the value to which it is programmed. Then you must press the button again to modify the value, when you find it press the S3-A3 button to program the new value.
  

****************************
### Contact

Any doubt, bug, apport or comment please contact me at danielbernalb@gmail.com or twitter account twitter [@danielbernalb](https://twitter.com/danielbernalb)

****************************

### Other recommended open projects of CO2 sensors or that include CO2 sensors:
- [CanAirIO Citizen network for monitoring air quality](https://canair.io/es/index.html).
- [M5Stack ESP32 Core Ink + SCD30](https://github.com/hpsaturn/co2_m5coreink).
- [Codos](https://github.com/miguelangelcasanova/codos).
- [Anaire](https://github.com/anaireorg/anaire-devices).
- [eMariete](https://emariete.com/en/home-co2-meter/).
- [Medición de CO2](http://www.jorgealiaga.com.ar/?page_id=2864).
