# LibreCO2
LibreCO2: Medidor de CO2 usando Arduino UNO y un sensor de CO2 comercial (Sensirion SCD30, Winsen MH-Z14 o MHZ-19 o Cubic CM1106).

Esta es una versión básica de un medidor de CO2 de bajo costo con los materiales más comunes del mercado: Arduino UNO y los 3 sensores de bajo costo de CO2 más populares: Sensirion SCD30, Winsen MH-Z14 o 19 y Cubic CM1106. LibreCO2 usa un Arduino UNO, pero puede extenderse al MEGA o nano, por ser muy popular en escuelas y frecuentemente usado en las clases de tecnología o electrónica y la mayoría de escuelas tienen muchos de ellos. 

El código del Arduino es lo más sencillo posible y puede programarse cargando el archivo .hex con el programa Xloader y así no necesitarías instalar el software Arduino para compilar y programar el código. Si buscas un sensor más avanzado y con conectividad por Bluetooth o por Wifi puedes encontrar un listado interesante al final de esta guía con varias iniciativas abiertas que incluye [CanAirIO](https://github.com/kike-canaries/canairio_firmware) con conectividad Bluetooth y Wifi. LibreCO2 usa los componentes más populares del mercado, así no sean los más avanzados, y el armado más sencillo posible sin necesitar el uso de cautín y soldadura de estaño.

Todos los sensores usados aquí son NDIR que es el estándar actuales para mediciones reales de CO2: 

https://www.co2meter.com/blogs/news/6010192-how-does-an-ndir-co2-sensor-work


**Materiales:**

Existen dos opciones para el armado de sensor, la más sencilla es usando un Escudo Multifunción (Shield) de Arduino el cual ya incluye el Display de 4 dígitos, el buzzer y los botones, con lo cual se facilita mucho más el montaje. La segunda armando las partes por individual.

# 1. Opción con Escudo multifunción:

![Materiales](https://github.com/danielbernalb/LibreCO2/blob/main/images/Materiales%20sensor.jpg)

1. Escudo multifunción (Arduino Shield multifunction).

El Escudo tiene un bug o error que puede cortocircuitarlo con el conector USB, corte los pines que se muestran en la foto y agregue un trozo de cinta al conector USB (que se muestra en el video):

![Cortar pines escudo](https://github.com/danielbernalb/LibreCO2/blob/main/images/Corte%20pines%20escudo.jpg)

2. Arduino UNO cualquier versión. Sirve el original o las versiones chinas, la diferencia entre los 2 está en la instalación del driver, ambos trabajan bien. Original a la derecha y copia a la izquierda:

![Arduino original & clone](https://github.com/danielbernalb/LibreCO2/blob/main/images/arduino-uno-original-clone.jpg)

3. 4 cables jumper hembra hembra.

4. Sensor, opciones:

	a. Sensirion SCD30, el más costoso (52 dólares) pero en nuestras pruebas el de mejor desempeño. Sensirion tiene distribuidores en USA y Europa que envían a Latinoamérica. Ejemplo:
	https://www.mouser.com/ProductDetail/Sensirion/SCD30/?qs=rrS6PyfT74fdywu4FxpYjQ%3D%3D
	
	![SCD30 Sensirion](https://github.com/danielbernalb/LibreCO2/blob/main/images/Sensirion%20SCD30.jpg)
	
	A favor: excelente desempeño (rápido y fiel), rápido envío en USA y Europa y envíos a Latinoamérica.
	
	En contra: precio (52 dólares). 
	
	Para la conexión del Arduino al sensor Sensirion se usa el protocolo Modbus ya que este nos permite conectar directamente el Arduino al SCD30 sin necesidad de drivers de datos que complicarían mucho el montaje. Sólo debes conectar el pin SEL al VIN del sensor usando una resistencia de 100 kΩ (kilo ohmnios) como se vé en las fotos y video para habilitar el modo Modbus.

	b. Winsen MH-Z14 o 19, el más barato y popular, buen desempeño, algo lento. Ten cuidado con las falsas copias del sensor!!!

	![Winsen MH-Z14a](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z14A.jpg)
	
	A favor: desempeño aceptable (lento comparado al Sensirion, fiabilidad media), bajo costo (18 dolares con envío incluido desde China por Aliexpress).
	
	En contra: tiempo de envío variable con Aliexpress desde China (20 días a mes y medio), falsificaciones (ten cuidado, sólo compra de la tienda Oficial de Aliexpress).

	REPITO: sólo compra el MH-Z19 de la tienda de distribución de Winsen en Aliexpress, el mercado está invadido de falsas copias, más información en: https://emariete.com/sensores-co2-mh-z19b-falsos/ o en ingles en: https://youtu.be/5_QQe75-SZI

	![Original Winsen MH-Z19b](https://github.com/danielbernalb/LibreCO2/blob/main/images/MH-Z19B.jpg)

	Venta de versiones originales en la página del fabricante en Aliexpress:
	https://es.aliexpress.com/item/1005001865093513.html

	c. Cubic CM1106, la última opción porque el distribuidor de China vende sólo unidades recuperadas de segunda mano y de vez en cuando están agotados, son muy baratos y su desempeño es aceptable. 
	https://www.aliexpress.com/item/4001082699057.html
	
	![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Cubic%20CM1106.jpg)
	
****************************

**Conexiones para cada sensor:**

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

GND ---> Pin 5 or 16 del conector

+5 &nbsp; ---> Pin 4 o 17 del conector

5 &nbsp; &nbsp; ---> Pin 3 o 18 del conector

6 &nbsp; &nbsp; ---> Pin 2 o 19 del conector

![MH-Z14A connection](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20Shield%20MHZ14%20connection.jpg)	


****************************
**Cubic CM1106 sensor**

GND ---> G Pin 2 de la regleta de 4 pines

+5 &nbsp; ---> V1 Pin 1 de la regleta de 4 pines

5 &nbsp; &nbsp; ---> R Pin 2 de la regleta de 5 pines

6 &nbsp; &nbsp; ---> T Pin 3 de la regleta de 5 pines

![Cubic CM1106](https://github.com/danielbernalb/LibreCO2/blob/main/images/Arduino%20Shield%20Cubic%20connection.jpg)

## Video explicativo:

[![](http://img.youtube.com/vi/KYHC06xhUu4/0.jpg)](http://www.youtube.com/watch?v=KYHC06xhUu4 "VideoLibreCO2")

# 2. Opción por partes individuales:

1. Arduino UNO original o versión china.
2. Display TM1687.
3. Jumper hembra - macho.
4. Uno o dos Pulsadores o cables (uno para la calibración, otro para el nivel de Beep).
5. Opcional: Buzzer.

![Materiales](https://github.com/danielbernalb/LibreCO2/blob/main/images/Materials%20all%20text.jpg)

6. Sensor, iguales opciones que la opción con Escudo multifunción.

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


**Pulsador de calibración**

Cualquiera ---> 2

Cualquiera ---> 4


**Pulsador de nivel del Beep**

Cualquiera ---> A5

Cualquiera ---> A3



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
# 3. Programar el Firmware (codigo en el Arduino)

Existen 3 opciones para programar el Arduino: la más sencilla es usar un teléfono Android y un cable USB OTG, la siguiente es usar un PC con el software Xloader y la última instalando Arduino en un PC.

**1. Teléfono Android.**

![USB OTG cable](https://github.com/danielbernalb/LibreCO2/blob/main/images/Cable%20USB%20OTG-002.jpg)

 - Conecta el cable USB OTG al teléfono.
 - Instala la app de Android ["Arduino Hex Uploader-Firmware Bin Upload"](https://play.google.com/store/apps/details?id=xyz.vidieukhien.embedded.arduinohexupload&hl=en_US&gl=US).
 - Baja el archivo .hex file de este ["Directorio de Google drive"](https://drive.google.com/drive/folders/1FxXLVhaGF6UCVObQlh8cVLddyWyQqUT0) basado en la versión de tu sensor (Shield y marca de sensor). Encuentra el archivo .hex que vas cargar en el Arduino, cada versión de sensor (Sensirion, Winsen or Cubic) se encuentra disponible el el directorio "hex files". Ejemplo: "LibreCO2_Shield_Sensirion.hex" es el arhivo para el sensor Sensirion con el Shield Multifunción. Nota: No descargues el archivo .hex directamente de la página con la opción "Salvar como", que daña el archivo. Usa solamente el enlace de Google Drive.
 - Busca el archivo .hex en tú teléfono.
 - Sube el codigo a tu tarjeta Arduino.
 - FIN.

Video de programación de el Firmware:

[![](http://img.youtube.com/vi/_87qzZFMgwg/0.jpg)](http://www.youtube.com/watch?v=_87qzZFMgwg "LibreCO2")

**2. Usando tu PC con el software Xloader (sólo Windows):**

1. Conecta el Arduino a tu computador y espera a que el driver se instale automáticamente. Si el driver no se instala automáticamente debes identificar si tu Arduino es original o una copia:

1.1. Si el Arduino es original, instala el paquete Arduino con drivers: https://www.arduino.cc/en/Guide/ArduinoUno

1.2. Si el Arduino es copia china sigue las instrucciones de esta página web: https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers

2. Después de instalado el driver, tienes 2 opciones:

2.1. Si lo único que quieres es programar el Arduino desde Windows sin tener que ver el código y compilarlo, usa el software Xloader que se encuentra en la carpeta del mismo nombre:

![Xloader](https://github.com/danielbernalb/LibreCO2/blob/main/images/Xloader1.jpg)

A la izquierda se muestra la ventana del Xloader cuando se inicia.

2.1.1. Explora hasta encontrar el achivo .hex que vas a cargar en el Arduino. Cada versión de sensor (Sensirion, Winsen o Cubic) se encuentra disponible en el directorio "hex files". Ejemplo: "LibreCO2_Shield_SCD30.hex" es el archivo para el sensor Sensirion SCD30 con el uso del Escudo Multifunción (Shield). No lo descargues directamente de la página con la opción "Guardar como", eso arruina el archivo.  Usa el siguiente enlace de Google Drive para bajar el archivo .hex bucando tu versión de Sensor (SCD30, MHZ14_9 or CM1106) y construcción (con Shield o por Partes):
 
https://drive.google.com/drive/folders/1FxXLVhaGF6UCVObQlh8cVLddyWyQqUT0

2.1.2. Selecciona la tarjeta Arduino que estas usando (Uno/ATmega328).

2.1.3. Selecciona el puerto de comunicaciones COM al que está conectado el Arduino, si no lo sabes buscalo en el Admnistrador de dispositivos "Puertos COM y LPT".

2.1.4. Selecciona la velocidad Baud rate a 115200.

2.1.5. Presiona el boton “Upload”.

2.1.6. Cuando el archivo .hex file se carga en el Arduino, foto a la derecha, sale el mensaje de "xxxxx bytes uploaded", si falla te saldrá el mensaje de "upload fails".

**3. Si quieres editar y compilar el código instala el software Arduino en Windows, Mac o Linux:**
  
  https://www.arduino.cc/en/software
  
  La siguiente guía explica como instalar Arduino en Windows, Mac o Linux y es muy completa:
  https://learn.sparkfun.com/tutorials/installing-arduino-ide/all  
  
  En Youtube existen cientos de tutoriales para instalar Arduino en diferentes sistemas operativos y aprender su programación, además puedes colaborar mejorando este código o crear tu propia versión.  
  
 # 4. Funcionamiento del sensor.
 
 Cuando se haya realizado la programación del Arduino tenemos varios modos y opciones de funcionamiento del sensor:
 
 1. Modo normal: Al encender el sensor aparece el aviso BEEP y un número entre 700 y 1300 que muestra el nivel de la alarma que tenemos programado. En el numeral 3 se explicará cómo modificarlo. Luego de ello el medidor verifica la conexión al sensor de bajo costo. Si aparece el mensaje "good" la conexión está bien hecha y se continuará con el proceso de calentamiento del sensor ("heat") que dura 30 segundos para los sensores Sensirion y Cubic, y 3 minutos para los Winsen. Si el mensaje es "fail" la conexión ha fallado y debemos verificar la conexión entre el Arduino y el sensor. Después de el calentamiento el valor de CO2 en partes por millón ppm, aparece en pantalla. Cuando el valor es superior al nivel Beep la alarma suena y los leds del escudo titilan.

 2. Modo Calibración: [Explicado en el video anexo](#video-explicativo).
La calibración del sensor se recomienda después de armado el sensor y cuando se aprecien mediciones muy diferentes al aire libre del rango estimado de 400 ppm. Todos estos sensores de bajo costo se calibran al aire libre, colocándolos en un espacio exterior por unos minutos sin que reciban viento fuerte pues esto afecta la medición, por ello se recomienda colocarlos dentro de una caja que evite el viento directo.
Luego de cumplir con estas condiciones, debes presionar el botón S1-A1 en la versión con escudo, o el "switch" en la versión por partes, durante más de 5 segundos, y aparecerá en pantalla el mensaje "cal-" y comenzando un conteo regresivo de 5 minutos para el Sensirion y Cubic y 20 minutos para el Winsen.
Al finalizar este tiempo el sensor recibe la orden de calibración y queda listo para usarse de nuevo. Si quieres interrumpir el proceso de calibración presiona el botón S3-A3.

3. Modo cambio de nivel del umbral de alarma BEEP: [Explicado en el video anexo](#video-explicativo).
Para cambiar el nivel de la alarma de 1000 ppm, por defecto, a otro valor entre 700 y 1300 ppm debes presionar el botón S2-A2 en la versión con escudo, o colocar un cable o switch adicional del pin A3 a A5 en la versión por partes, durante más de 5 segundos y aparecerá el mensaje "BEEP" y luego el valor al que esta programado. Luego debes presionar de nuevo el botón para modificar el valor, cuando lo encuentres presiona el botón S3-A3 para programar este nuevo valor de nivel del BEEP.
  
  
****************************
### Contacto

Si tienes alguna duda, bug, aporte o comentario escríbeme al correo electrónico danielbernalb@gmail.com o la cuenta de twitter [@danielbernalb](https://twitter.com/danielbernalb) 

****************************

### Otros proyectos abiertos recomendados de sensores de CO2 o que incluyen sensores de CO2:

- [CanAirIO Citizen network for monitoring air quality](https://canair.io/es/index.html).
- [M5Stack ESP32 Core Ink + SCD30](https://github.com/hpsaturn/co2_m5coreink).
- [Codos](https://github.com/miguelangelcasanova/codos).
- [Anaire](https://github.com/anaireorg/anaire-devices).
- [eMariete](https://emariete.com/en/home-co2-meter/).
- [Medición de CO2](http://www.jorgealiaga.com.ar/?page_id=2864).
