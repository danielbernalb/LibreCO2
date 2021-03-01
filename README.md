# LibreCO2
LibreCO2: Simple CO2 meter using Arduino UNO, display, buzzer and CO2 sensor (Sensirion SCD30, Winsen MH-Z14 or MHZ-19 and Cubic CM1106).

  This is a very simple version of a low cost CO2 meter with the most common materials on the market: an Arduino UNO or Leonardo, a 4 digits TM1687 display, a common buzzer and the three most popular low cost real CO2 sensor: Sensirion SCD30, Winsen MH-Z14 or 16 and the Cubic CM1106. LibreCO2 uses an Arduino UNO or NANO, that is very popular in schools and frequently used in the "technology class" and the majority of schools have many of them. The code is the simplest possible and the .hex file is published for the programming of the Arduino with Xloader and you don{t have to install Arduino software and compile the code.

All sensors used are NDIR that is the actual standard for real CO2 measurements:
https://en.wikipedia.org/wiki/Carbon_dioxide_sensor
https://www.co2meter.com/blogs/news/6010192-how-does-an-ndir-co2-sensor-work

Materials:

1. Arduino UNO original or chinese version, the difference is the driver installation, both are very easy to find in any country and work well.

![Arduino original & clone](https://github.com/danielbernalb/LibreCO2/blob/main/images/arduino-uno-original-clone.jpg)
       
   Original at right and clone at left

2. 4 digits display TM1687.

![4 digits display TM1687](https://github.com/danielbernalb/LibreCO2/blob/main/images/Display-TM1687.jpg)

3. Jumper female - male.

![Jumper FM](https://github.com/danielbernalb/LibreCO2/blob/main/images/Jumper.jpg)

4. Optional: buzzer.

![Big and small buzzer](https://github.com/danielbernalb/LibreCO2/blob/main/images/big-small-buzzer.jpg)

5. Sensor, options:

	a. Sensirion SCD30, the most expensive (52 dollars) but in our test the best performance. Sensirion has distributors in the USA and Europe. Example:
	https://www.mouser.com/ProductDetail/Sensirion/SCD30/?qs=rrS6PyfT74fdywu4FxpYjQ%3D%3D
	
	![SCD30 Sensirion](https://github.com/danielbernalb/LibreCO2/blob/main/images/Sensirion%20SCD30.jpg)
	
	Pros: excellent performance (fast, reliable), fast shipping from USA or Europe.
	
	Cons: price (52 dollars) requieres a level driver (BSS138 type) for secure connection with the Arduino. 

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

Electronic component ---> Arduino PIN

**Display---> Arduino PIN**

CLK    ---> 9

DIO    ---> 8

VCC    ---> IOREF in original or 5V in clone

GND    ---> GND


**Buzzer**

"+"    ---> 11

other  ---> GND


**Button**

Anyone ---> 2

Anyone ---> 4



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
**Program the Firmware (code in the Arduino)**

1. Connect the Arduino to your computer. If the driver is not automatically detected you have to identify if your Arduino is original or chinese clone:

1.1. If the Arduino is original, install Arduino with drivers: https://www.arduino.cc/en/Guide/ArduinoUno

1.2. If the Arduino is chinese clone please follow the instructions in this web page: https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers

2. You have two options:

2.1. The easy way if you don't want to install Arduino, load and compile the code, only you need to programming, use Xloader:

How to use Xloader to load a new version of grbl into the Arduino/X-controller.

![Xloader](https://github.com/danielbernalb/LibreCO2/blob/main/images/Xloader1.png)

The left screenshot shows the window that Xloader displays when it starts.

Enter the file path or browse to the .hex file that you would like to load into the Arduino. The files are available in each directory of the Sensor version used (Sensirion, Winsen or Cubic).

Select the device type you are going to load into (Uno/ATmega328).

Select the COM port where your Arduino is attached.

The baud rate should be 115200.

Press the “Upload” button.

When the .hex file has been loaded into the Arduino the right screenshot will show the size of the file uploaded, or an error message if the upload fails.


2.2. Install Arduino and compile the code.
  
  https://www.arduino.cc/en/software
  
  Instructions to compile and burn the code:
  
  https://youtu.be/fJWR7dBuc18
  
  

****************************
**Contact**

Any doubt, bug, apport or comment please contact me at danielbernalb@gmail.com or twitter account @danielbernalb



