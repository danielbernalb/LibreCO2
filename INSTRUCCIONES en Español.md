# LibreCO2
LibreCO2: Medidor de CO2 simple usando un Arduino UNO, un display 7 segmentos, un buzzer o chicharra y un sensor de CO2 (Sensirion SCD30, Winsen MH-Z14 o MHZ-19 y Cubic CM1106).

Esta es una versión simple y básica de un medidor de bajo costo de CO2 con los materiales más comúnmente encontrados en el mercado: un Arduino UNO o Leonardo, un display de 4 dígitos TM1687, un buzzer o chicharra pasiva y los 3 sensores de bajo costo de CO2 más populares: Sensirion SCD30, Winsen MH-Z14 o 19 y el Cubic CM1106. LibreCO2 usa un Arduino UNO pero puede extenderse a un NANO o MEGA, que son muy populares en escuelas y frecuentemente usados en las clases de tecnología o electrónica. El código del Arduino es lo más sencillo posible y puede programarse cargando el archivo .hex con el programa Xloader y así no necesitarías instalar el software Arduino para compilar el código. Si buscas conectividad por Bluetooth o por Wifi aquí en github puedes encontrar varias versiones. El medidor LibreCO2 usa los componentes más populares del mercado, así no sean los más avanzados, y el armado más sencillo posible sin necesitar del uso de cautín y soldadura de estaño.

Todos los sensores usados aquí son NDIR que es el actual estándar para mediciones reales de CO2: 

https://en.wikipedia.org/wiki/Carbon_dioxide_sensor

https://www.co2meter.com/blogs/news/6010192-how-does-an-ndir-co2-sensor-work


Materiales:

1. Arduino UNO original o versión china, la diferencia entre los 2 está en la instalación del driver, ambos son populares en cualquier país y trabajan bien.

![Arduino original & clone](https://github.com/danielbernalb/LibreCO2/blob/main/images/arduino-uno-original-clone.jpg)
       
   Original a la derecha y copia a la izquierda

2. Display TM1687 de 4 dígitos.

![4 digits display TM1687](https://github.com/danielbernalb/LibreCO2/blob/main/images/Display-TM1687.jpg)

Prueba

![Materiales](https://github.com/danielbernalb/LibreCO2/blob/main/images/Materiales1.jpg)

3. Jumper hembra - macho.

![Jumper FM](https://github.com/danielbernalb/LibreCO2/blob/main/images/Jumper.jpg)

4. Opcional: Pulsador o cable.

![Pulsador](https://github.com/danielbernalb/LibreCO2/blob/main/images/Pulsador%20largo.jpg)

5. Opcional: buzzer o chicharra.

![Big and small buzzer](https://github.com/danielbernalb/LibreCO2/blob/main/images/big-small-buzzer.jpg)

6. Sensor, opciones:

	a. Sensirion SCD30, el más costoso (52 dollars) pero en nuestras pruebas el de mejor performance. Sensirion tiene distribuidores en USA y Europa que envían a Latinoamérica.. Ejemplo:
	https://www.mouser.com/ProductDetail/Sensirion/SCD30/?qs=rrS6PyfT74fdywu4FxpYjQ%3D%3D
	
	![SCD30 Sensirion](https://github.com/danielbernalb/LibreCO2/blob/main/images/Sensirion%20SCD30.jpg)
	
	A favor: excelente performance (rápido y fiel), rápido envío en USA y Europa y envíos a Latinoamérica.
	
	En contra: precio (52 dólares). 
	
	Para la conexión del Arduino al sensor Sensirion se usa el protocolo Modbus ya que este nos permite conectar directamente el Arduino al SCD30 sin necesidad de drivers externos que complicarían mucho el montaje. Sólo debes conectar el pin SEL al VIN del sensor como se vé en el gráfico  para habilitar el modo Modbus.

	b. Winsen MH-Z14 o 19, el más barato y popular, buen performance, algo lento. Ten cuidado con las falsas copias del sensor!!!

	![Winsen MH-Z14a](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z14A.jpg)
	
	A favor: performance aceptable (lento comparado al Sensirion, fiabilidad media), bajo costo (18 dolares con envío incluido desde China por Aliexpress).
	
	En contra: tiempo de envío variable con Aliexpress desde China (20 días a mes y medio), falsificaciones (ten cuidado, sólo compra de la tienda Oficial de Aliexpress).

	REPITO: sólo compra el MH-Z19 de la tienda de distribución de Winsen en Aliexpress, el mercado está invadido de falsas copias, más información en: https://emariete.com/sensores-co2-mh-z19b-falsos/ o en ingles en: https://youtu.be/5_QQe75-SZI

	![Original Winsen MH-Z19b](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z19B.jpg)

	Originales en Aliexpress:
	https://es.aliexpress.com/item/1005001865093513.html

	c. Cubic CM1106, la última opción porque el distribuidor de China vende sólo unidades recuperadas de segunda mano y de vez en cuando están agotados, son muy baratos y su performance es aceptable. 
	https://www.aliexpress.com/item/4001082699057.html
	
	![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Cubic%20CM1106.jpg)
	

****************************


**Conexiones para todos los sensores:**

Componente electrónico ---> PIN del Arduino

**Display---> PIN Arduino**

CLK    ---> 9

DIO    ---> 8

VCC    ---> IOREF en la versión original o 5V en la copia china

GND    ---> GND


**Buzzer**

"+"    ---> 11

otro  ---> GND


**Button**

Cualquiera ---> 2

Cualquiera ---> 4



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

1. Conecta el Arduino a tu computador y espera a que el driver se instale automáticamente. Si el driver no se instala automáticamente debes identificar si tu Arduino es original o una copia:

1.1. Si el Arduino es original, instala el paquete Arduino con drivers: https://www.arduino.cc/en/Guide/ArduinoUno

1.2. Si el Arduino es copia sigue las instrucciones de esta página web: https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers

2. Después de instalado el driver, tienes 2 opciones:

2.1. Si lo único que quieres es programar el Arduino sin tener que ver el código, compilarlo y luego programarlo, usa Xloader:

![Xloader](https://github.com/danielbernalb/LibreCO2/blob/main/images/Xloader1.jpg)

A la izquierda se muestra la ventana del Xloader cuando se inicia.

2.1.1. Explora hasta encontrar el achivo .hex que vas a cargar en el Arduino. El archivo .hex se encuentra en los directorios de este repositorio de cada modelo de sensor (Sensirion, Winsen or Cubic).

2.1.2. Selecciona la tarjeta Arduino que estas usando (Uno/ATmega328).

2.1.3. Selecciona el puerto de comunicaciones COM al que está conectado el Arduino.

2.1.4. Selecciona la velocidad Baud rate a 115200.

2.1.5. Presiona el boton “Upload”.

2.1.6. Cuando el archivo .hex file se carga en el Arduino, foto a la derecha, sale el mensaje de "xxxxx bytes uploaded", si falla te saldrá el mensaje de "upload fails".

2.2. Si quieres editar y compilar el código instala el software Arduino:
  
  https://www.arduino.cc/en/software
  
  Instrucciones para compilar y programar el código:
  
  https://youtu.be/fJWR7dBuc18
  
  

****************************
**Contacto**

Si tienes alguna duda, bug, aporte o comentario escríbeme al correo eléctronico danielbernalb@gmail.com o la cuenta de twitter [@danielbernalb](https://twitter.com/danielbernalb) 



