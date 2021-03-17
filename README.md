# LibreCO2
LibreCO2: Simple CO2 meter using Arduino UNO and CO2 low cost sensor (Sensirion SCD30, Winsen MH-Z14 or MHZ-19 and Cubic CM1106).

 This is a simple and low cost sensor version of a CO2 meter built with the most common materials on the market: an Arduino UNO and the three most popular low cost REAL CO2 sensors: Sensirion SCD30, Winsen MH-Z14 or 19 and Cubic CM1106. LibreCO2 uses an Arduino UNO but can be extended to MEGA or nano, that are very popular in schools and frequently used in technology and the electronics classes. Also, the majority of schools have many of them.
  
  The code is the most basic and the .hex file is published for the programming of the Arduino with the software Xloader.  You don't have to install Arduino software to compile and program the code. If you are searching for Wi-Fi or Bluetooth connectivity , you can find many open source alternatives on a list at the end of the guide, for example [CanAirIO](https://github.com/kike-canaries/canairio_firmware). The CO2 meter uses the most popular components on the market, not the most advanced, and the simplest building without the need of an iron solder.

All the sensors used are NDIR , which is the actual standard for real CO2 measurements:

https://www.co2meter.com/blogs/news/6010192-how-does-an-ndir-co2-sensor-work

**Materials:**

There are two options for the sensor assembly: the easiest is using an Arduino Multifunction Shield (Shield),  which already includes a 4-digit display, buzzer and buttons, making the assembly much easier. The second is assembling the parts individually.

# 1. Multi-function Shield option:

![Materials](https://github.com/danielbernalb/LibreCO2/blob/main/images/Materiales%20sensor.jpg)

1. Arduino Multifunction Shield.

2. Arduino UNO, the original or Chinese version. The difference is the driver installation. Both are very easy to find in any country and work well. The original (right) and clone (left):

![Arduino original & clone](https://github.com/danielbernalb/LibreCO2/blob/main/images/arduino-uno-original-clone.jpg)

3. (4) Jumper cables, female - female.

4. Sensor, options:

	a. Sensirion SCD30, is the most expensive (US$52) but delivers the best performance. Sensirion has distributors in  USA and Europe. Example:
	https://www.mouser.com/ProductDetail/Sensirion/SCD30/?qs=rrS6PyfT74fdywu4FxpYjQ%3D%3D
	
	![SCD30 Sensirion](https://github.com/danielbernalb/LibreCO2/blob/main/images/Sensirion%20SCD30.jpg)
	
	Pros: excellent performance (fast & reliable), expedited shipping from USA or Europe.
	
	Cons: price (52 dollars). 
	
	The Modbus protocol is used for the connection between the Arduino and the sensor which allows the direct connection between the Arduino and the sensor without the use of a hardware data driver. You must connect the SEL pin with the VIN pin of the sensor using a 100 kΩ (kilo ohms) resistance as seen in the pictures and video to activate the Modbus protocol.
	
	b. Winsen MH-Z14 or 19, one of the cheapest and most popular, good performance, some maybe slow. Beware of fake clones!!!

	![Winsen MH-Z14a](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z14A.jpg)
	
	Pros: acceptable performance (slow compared with Sensirion, medium reliable), low price (18 dollars including shipping from Aliexpress)
	
	Cons: slow shipping with Aliexpress, standard shipping (6-20 weeks), fake copies (you have to be careful, its recommended only buying from an official store).

	Only buy MH-Z19 from the Winsen Supplier in Aliexpress, the market is flooded with fake copies, more info in: https://youtu.be/5_QQe75-SZI or in spanish https://emariete.com/sensores-co2-mh-z19b-falsos/

	![Original Winsen MH-Z19b](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z19B.jpg)

	Original at Aliexpress:
	https://es.aliexpress.com/item/1005001865093513.html

	c. Cubic CM1106, is the last option because the distribution in China is for second-hand units and sometimes not available.
	https://www.aliexpress.com/item/4001082699057.html
	
	![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Cubic%20CM1106.jpg)
	

****************************

**Connections for each sensor:**

**Sensirion SCD30**

Shield PIN ---> Sensor

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

GND ---> 5 or 16 pin connector

+5 &nbsp; ---> 4 or 17 pin connector

5 &nbsp; &nbsp; ---> 3 o 18 pin connector

6 &nbsp; &nbsp; ---> 2 o 19 pin connector

![MH-Z14A connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20Shield%20MHZ14%20connection.jpg)	


****************************
**Cubic CM1106 sensor**

GND ---> G Pin 2 a 4-pin connector

+5 &nbsp; ---> V1 Pin 1 a 4-pin connector

5 &nbsp; &nbsp; ---> R Pin 2 a 5-pin connector

6 &nbsp; &nbsp; ---> T Pin 3 a 5-pin connector

![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20Shield%20Cubic%20connection.jpg)

## Assembly video
(in Spanish with English subtitles):

[![](http://img.youtube.com/vi/KYHC06xhUu4/0.jpg)](http://www.youtube.com/watch?v=KYHC06xhUu4 "VideoLibreCO2")

# 2. Individual parts option:

1. Arduino UNO original or Chinese.
2. Display TM1687.
3. Jumper female - male.
4. One or two buttons switches or cables (one for the calibration, the other for the Beep level).
5. Optional: Buzzer.

![Materials](https://github.com/danielbernalb/LibreCO2/blob/main/images/Materials%20all%20text.jpg)

6. Sensor, same options as Multi-function Shield option.

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

VIN Pin 1 a 4-pin connector ---> 5V 

GND Pin 2 a 4-pin connector ---> GND

RX Pin 2 a 5-pin connector ---> 6

TX Pin 3 a 5-pin connector ---> 7 


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

V1 Pin 1 a 4-pin connector ---> 5V 

G Pin 2 a 4-pin connector ---> GND

R Pin 2 a 5-pin connector ---> 6

T Pin 3 a 5-pin connector ---> 7 

![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20%20CM1106%20connection.jpg)

****************************
# 3. Program / Burn the Firmware (the code in the Arduino)

There are three options for the program of the Arduino: the easiest is using an Android cell phone and an USB OTG cable, the second is using a PC with the software Xloader and the last is installing Arduino on your PC.

**1. Android cell phone**

![USB OTG cable](https://github.com/danielbernalb/LibreCO2/blob/main/images/Cable%20USB%20OTG-002.jpg)

 - Connect the USB OTG cable to the cell phone.
 - Install the Android app ["Arduino Hex Uploader-Firmware Bin Upload"](https://play.google.com/store/apps/details?id=xyz.vidieukhien.embedded.arduinohexupload&hl=en_US&gl=US)
 - Dowload the .hex file from this ["Google drive directory"](https://drive.google.com/drive/folders/1FxXLVhaGF6UCVObQlh8cVLddyWyQqUT0) based on your sensor version (Shield and brand sensor). Find the .hex file that you are going to load into the Arduino, each sensor version (Sensirion, Winsen or Cubic) is available in the "hex files" directory. Example: "LibreCO2_Shield_Sensirion.hex" is the file for the Sensirion sensor with the Multifuntion Shiedl version. Note: Don't download it directly from the page with the "Save As" option, that ruins the file. Use only the Google Drive link.
 - Browse the .hex file in your cell phone.
 - Upload the code to your Arduino board.
 - FINISH.

Programming / burning video (in Spanish with English subtitles):

[![](http://img.youtube.com/vi/_87qzZFMgwg/0.jpg)](http://www.youtube.com/watch?v=_87qzZFMgwg "LibreCO2")

**2. Using your PC with Xloader software (only Windows):**

1. Connect the Arduino to your computer. If the driver is not automatically detected , you will have to identify if your Arduino is the original or a Chinese clone:

1.1. If the Arduino is a Chinese version, please follow the instructions on this web page: https://www.arduino.cc/en/Guide/ArduinoUno

1.2. If the Arduino is chinese version please follow the instructions in this web page: https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers

2. After installing the driver, you have 2 options:

2.1. The easy option is to use the Xloader software in the folder, of the repository, with the same name:

![Xloader](https://github.com/danielbernalb/LibreCO2/blob/main/images/Xloader1.jpg)

The left screenshot shows the window that Xloader displays when it starts.

Explore until you find the .hex file that you are going to load into the Arduino. Each sensor version (Sensirion, Winsen or Cubic) is available in the "hex files" directory. Example: "LibreCO2_Shield_SCD30.hex" is the file for the Sensirion SCD30 sensor with the Multifuntion Shield version. Note: Don't download it directly from the page with the "Save As" option, that ruins the file. Use this ["Google Drive link"](https://drive.google.com/drive/folders/1FxXLVhaGF6UCVObQlh8cVLddyWyQqUT0) to download your Sensor version (SCD30, MHZ14_9 or CM1106) and build option (with Shield or by Parts).

2.1.2. Select the device type you are going to load into (Uno/ATmega328).

2.1.3. Select the COM port where your Arduino is attached, if you don't know, look for it in the Device Manager "COM and LPT ports".

2.1.4. The baud rate should be 115200.

2.1.5. Press the “Upload” button.

2.1.6. When the .hex file has been loaded into the Arduino, the right screenshot, the message of "xxxxx bytes uploaded" appears, if it fails you will get the message of "upload fail"

**3. If you want to edit and compile the code please install the Arduino software in Windows, Mac or Linux:**
  
  https://www.arduino.cc/en/software
  
  The following guide explains how to install Arduino on Windows, Mac or Linux and is very explanatory:
  
  https://learn.sparkfun.com/tutorials/installing-arduino-ide/all  
  
  YouTube contains hundreds of tutorials on how to install Arduino on different operating systems and on learning the programming. You can also collaborate by improving this code or create your own version.
  
  # 4. Sensor Operation
 
 When the Arduino programming has been completed, we have several sensor operating modes and options:
 
 1. Normal mode: When the sensor is turned on, the BEEP message appears and a number between 700 and 1300 shows the alarm level that we have programmed. Section 3 below explains how to modify it. After that,  the meter verifies the connection to the low cost sensor. If a "good" message appears, then connection is well made and the sensor heating process ("heat") will continue, which lasts 30 seconds for the Sensirion and Cubic sensors, and 3 minutes for the Winsen sensors. If the message is "fail" then the  connection failed and we must verify the connection between the Arduino and the sensor. After heating up, the CO2 value in parts per million ppm appears on the screen. When the value is higher than the Beep level, the alarm sounds and the shield LEDs flash.

 2. Calibration Mode: [Explained in the attached video](#assembly-video).
Sensor calibration is recommended after sensor assembly and when widely differing measurements are observed outdoors in the 400 ppm range. All these low-cost sensors are calibrated outdoors, placing them in an outdoor space for a few minutes without receiving any direct strong wind because it affects the measurement. Therefore, it is recommended to place them inside a box that avoids direct wind. After fulfilling these conditions, you must press the S1-A1 button in the shield version, or the "switch" in the version by parts, for more than 5 seconds, and the "cal-" message will appear on the screen, counting down 5 minutes for the Sensirion and Cubic and 20 minutes for the Winsen. At the end of this time, the sensor receives the calibration order and is ready to be used again. If you want to interrupt the calibration process, press the S3-A3 button.

3. BEEP alarm threshold level change mode: [Explained in the attached video](#assembly-video).
To change the alarm level from 1000 ppm, by default, to another value between 700 and 1300 ppm you must press button S2-A2 in the shield version, or place an additional cable or switch from pin A3 to A5 in the parts version, for more than 5 seconds and the message "BEEP" will appear, then the value to which it is programmed. Then you must press the button again to modify the value. When you find it, press the S3-A3 button to program this new BEEP level value.
  

****************************
### Contact

Should you have any doubts, bugs, or comments please do not hesitate to contact me at danielbernalb@gmail.com or at my twitter account [@danielbernalb](https://twitter.com/danielbernalb)

****************************

### Other recommended open CO2 sensor projects or projects that include CO2 sensors as follows:

- [CanAirIO Citizen network for monitoring air quality](https://canair.io/es/index.html).
- [M5Stack ESP32 Core Ink + SCD30](https://github.com/hpsaturn/co2_m5coreink).
- [Codos](https://github.com/miguelangelcasanova/codos).
- [Anaire](https://github.com/anaireorg/anaire-devices).
- [eMariete](https://emariete.com/en/home-co2-meter/).
- [Medición de CO2](http://www.jorgealiaga.com.ar/?page_id=2864).
