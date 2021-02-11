# LibreCO2
LibreCO2: Simple CO2 meter using Arduino UNO, display, buzzer and CO2 sensor (Sensirion SCD30, Winsen MH-Z14 or MHZ-19 and Cubic CM1106).

  This is a very simple version of a low cost CO2 meter with the most common materials on the market: an Arduino UNO or Leonardo, a 4 digits TM1687 display, a common buzzer and the three most popular low cost real CO2 sensor: Sensirion SCD30, Winsen MH-Z14 or 16 and the Cubic CM1106. LibreCO2 use an Arduino UNO or NANO, that is very popular in schools and frequently used in the "technology class" and the majority of schools have many of them. The code is the simplest posible and the .hex file is published for the programming of the Arduino with Xloader and you don{t have to install Arduino software and compilate the code.

All sensors used are NDIR that is the actual standard for real CO2 measurements:
https://en.wikipedia.org/wiki/Carbon_dioxide_sensor
https://www.co2meter.com/blogs/news/6010192-how-does-an-ndir-co2-sensor-work

Materials:

1. Arduino UNO original or chinese version, the difference is the driver installation, both are very easy to find in any country and work well.

![Arduino original & clone](https://github.com/danielbernalb/LibreCO2/blob/main/images/arduino-uno-original-clone.jpg)

Original at right and clone at left

2. 4 digits display TM1687.

![4 digits display TM1687](https://github.com/danielbernalb/LibreCO2/blob/main/images/Display-TM1687.jpg)

3. Optional: buzzer.

![Big and small buzzer](https://github.com/danielbernalb/LibreCO2/blob/main/images/big-small-buzzer.jpg)

4. Sensor, options:

	a. Sensirion SCD30, the most expensive (52 dollars) but in our test the best performance. Sensirion have distributors in USA and Europe. Example:
	https://www.mouser.com/ProductDetail/Sensirion/SCD30/?qs=rrS6PyfT74fdywu4FxpYjQ%3D%3D
	![SCD30 Sensirion](https://github.com/danielbernalb/LibreCO2/blob/main/images/Sensirion%20SCD30.jpg)

	b. Winsen MH-Z14 or 19, one of the chepears and most popular, good performance, some slow. Take care with fake clones!!!

	![Winsen MH-Z14a](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z14A.jpg)

	Only buy MH-Z19 from the Winsen Supplier in Aliexpress, the market is invaded with fake copies, more info in: https://youtu.be/5_QQe75-SZI or in spanish https://emariete.com/sensores-co2-mh-z19b-falsos/

	![Original Winsen MH-Z19b](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z19B.jpg)

	Originals in Aliexpress:
	https://es.aliexpress.com/item/1005001865093513.html

	c. Cubic CM1106, the last option because the distribution in China is for second hand units and sometimes is not available.
	https://www.aliexpress.com/item/4001082699057.html
	
	![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Cubic%20CM1106.jpg)


Connections:
Sensirion SCD30 model.

Sensor ---> Arduino PIN
VIN    ---> 5V 
GND    ---> GND
TX/SCL ---> SCL
RX/SDA ---> SDA

Display---> Arduino PIN
CLK    ---> 9 
DIO    ---> 8
VCC    ---> IOREF in original or 5V in clone
GND    ---> GND

Buzzer
+      ---> 13
-      ---> GND

Button Calibration
Anyone ---> 2
Anyone ---> 4


![SCD30 connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Connection%20Sensirion%20SCD30.jpg)
	
