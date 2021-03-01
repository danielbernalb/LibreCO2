# LibreCO2
LibreCO2: Medidors simple de CO2 usando un Arduino UNO, un display 7 segmentos, un buzzer o chicharra y un sensor de CO2 (Sensirion SCD30, Winsen MH-Z14 o MHZ-19 y Cubic CM1106).

  Esta es una versión muy simple y básica de un medidor de bajo costo de CO2 con los materiales más comunmente encontrados en el mercado: un Arduino UNO o Leonardo, un display de 4 digitos TM1687, un buzzer u chicharra pasiva y los 3 sensores de bajo costo más populares: Sensirion SCD30, Winsen MH-Z14 o 19 y el Cubic CM1106. 
  LibreCO2 usa un Arduino UNO pero puede extenderse a un NANO o MEGA, que son muy populares en escuelas y frecuentemente usandos en las clases de tecnología o electrónica. El código del Arduino es lo más sencillo posible y puede programarse cargando el archivo .hex con el programa Xloader y así no necesitarías instalar el software Arduino para compilar el código.

Todos los sensores usados son NDIR que es el actual estandar para mediciones reales de CO2:
https://en.wikipedia.org/wiki/Carbon_dioxide_sensor
https://www.co2meter.com/blogs/news/6010192-how-does-an-ndir-co2-sensor-work

Materiales:

1. Arduino UNO original or versión china, la diferencia entre los 2 está en la instalación del driver, ambos son populares en cualquier país y trabajan bien.

![Arduino original & clone](https://github.com/danielbernalb/LibreCO2/blob/main/images/arduino-uno-original-clone.jpg)
       
   Original a la derecha y clone a la izquierda

2. Display TM1687 de 4 digitos.

![4 digits display TM1687](https://github.com/danielbernalb/LibreCO2/blob/main/images/Display-TM1687.jpg)

3. Jumper hembra - macho.

![Jumper FM](https://github.com/danielbernalb/LibreCO2/blob/main/images/Jumper.jpg)

4. Opcional: buzzer o chicharra.

![Big and small buzzer](https://github.com/danielbernalb/LibreCO2/blob/main/images/big-small-buzzer.jpg)

5. Sensor, opciones:

	a. Sensirion SCD30, el más costoso (52 dollars) pero en nuestras pruebas el de mejor performance. Sensirion tiene distribuidores en USA y Europa que enván a Latinoamerica. Ejemplo:
	https://www.mouser.com/ProductDetail/Sensirion/SCD30/?qs=rrS6PyfT74fdywu4FxpYjQ%3D%3D
	
	![SCD30 Sensirion](https://github.com/danielbernalb/LibreCO2/blob/main/images/Sensirion%20SCD30.jpg)
	
	A favor: excelente performance (rapido y fiel), rapido envio en USA y Europa y envíoa a Latinoamerica.
	
	En contra: precio (52 dolares). 

	b. Winsen MH-Z14 or 19, el más barato y popular, buen performance, algo lento. Ten cuidado con las falsas copias del sensor!!!

	![Winsen MH-Z14a](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z14A.jpg)
	
	Pros: performance aceptable (lento comparado al Sensirion, fiabilidad media), bajo costo (18 dolares todo incluido desde China Aliexpress).
	
	Cons: envío lento con Aliexpress desde China (20 días a un mes y medio), falsificaciones (ten cuidado, sólo compra de la tienda Oficial en Aliexpress).

	Solo compra el MH-Z19 de la tienda de distribución de Winsen en Aliexpress, el mercado está invadido de falsas copias, más información en: https://emariete.com/sensores-co2-mh-z19b-falsos/ o en ingles en: https://youtu.be/5_QQe75-SZI

	![Original Winsen MH-Z19b](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z19B.jpg)

	Originales en Aliexpress:
	https://es.aliexpress.com/item/1005001865093513.html

	c. Cubic CM1106, la última opción porque el distribuidor de China vende sólo unidades recuperadas de segunda mano en China y de vez en cuando están agotados.
	https://www.aliexpress.com/item/4001082699057.html
	
	![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Cubic%20CM1106.jpg)
	

****************************


**Conecciones para todos los sensores:**

Componente electrónico ---> PIN del Arduino

**Display---> PIN Arduino**

CLK    ---> 9

DIO    ---> 8

VCC    ---> IOREF en la versión original o 5V en la copia china

GND    ---> GND


**Buzzer**

"+"    ---> 11

other  ---> GND


**Button**

Anyone ---> 2

Anyone ---> 4



****************************
**Sensor Sensirion SCD30**

VIN    ---> 3.3V 

GND    ---> GND

TX/SCL ---> 7

RX/SDA ---> 6

SEL    ---> 3.3V


![SCD30 connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20SCD30%20connection%20Modbus.jpg)


****************************
**Winsen MH-Z19B sensor**

VIN Pin 1 de regleta de 4 pines ---> 5V 

GND Pin 2 de regleta de 4 pines ---> GND

RX Pin 2 de regleta de 5 pines ---> 6

TX Pin 3 de regleta de 5 pines ---> 7 


![MH-Z19B connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20MHZ19%20connection.jpg)


****************************
**Winsen MH-Z14A sensor**

Pin 2 o 19 del conector ---> 6

Pin 3 o 18 del conector ---> 7

Pin 4 o 17 del conector ---> 5V

Pin 5 or 16 del conector ---> GND 


![MH-Z14A connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20MHZ14%20connection.jpg)	


****************************
**Cubic CM1106 sensor**

V1 Pin 1 de la regleta de 4 pines ---> 5V 

G Pin 2 de la regleta de 4 pines ---> GND

R Pin 2 de la regleta de 5 pines ---> 6

T Pin 3 de la regleta de 5 pines ---> 7 

![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20UNO%20%20CM1106%20connection.jpg)

****************************
**Programar el Firmware (codigo en el Arduino)**

1. Conecta el Arduino a tu computador. si el driver no se instala automáticamente debes identificar si tu Arduino es original o una copia:

1.1. Si el Arduino es original, instala el paquete Arduino con drivers: https://www.arduino.cc/en/Guide/ArduinoUno

1.2. Si el Arduino es copia sigue las instrucciones de esta página web: https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers

2. Despues de instalado el driver, tienes 2 opciones:

2.1. La fácil sin necesidad de instalar el software Arduino para cargar y compilar el código, si sólo necesitas programarlo, usa Xloader:

![Xloader](https://github.com/danielbernalb/LibreCO2/blob/main/images/Xloader1.png)

A la izquierda se muestra la ventana que muestra el Xloader cuando se inicia.

Inserta la ruta al archivo o explora hasta encontrar el achivo .hex que vas a cargar en el Arduino. El archivo .hex se encuentra en los directorios de cada modelo de sensor (Sensirion, Winsen or Cubic).

Selecciona la tarjeta Arduino que estas usando (Uno/ATmega328).

Selecciona el puerto de comunicaciones COM al que está conectado el Arduino.

Selecciona la velocidad Baud rate a 115200.

Presiona el boton “Upload”.

Cuando el archivo .hex file se carga en el Arduino, foto a la derecha, sale el mensaje de "xxxxx bytes uploaded", si falla te saldrá el mensaje de "upload fails".


2.2. Instalar el software Arduino para compilar y programar el código en la tarjeta, también puedes editarlo a tu gusto:
  
  https://www.arduino.cc/en/software
  
  Instrucciones para compilar y programar el código:
  
  https://youtu.be/fJWR7dBuc18
  
  

****************************
**Contacto**

Alguna duda, bug, aporte o comentario contactame al correo eléctronico danielbernalb@gmail.com o la cuenta de twitter @danielbernalb



