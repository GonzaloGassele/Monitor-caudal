/*  
 
    ESQUEMA DE CONEXION
    
                                                +-----+
                   +----[PWR]-------------------| USB |--+
                   |                            +-----+  |
                   |         GND/RST2  [ ][ ]            |
                   |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] |   
                   |          5V/MISO2 [ ][ ]  A4/SDA[ ] |   
                   |                             AREF[ ] |
                   |                              GND[ ] |
                   | [ ]N/C                    SCK/13[ ] |   Led
                   | [ ]IOREF                 MISO/12[ ] |   
                   | [ ]RST                   MOSI/11[ ]~|   
                   | [ ]3V3    +---+               10[ ]~|   Relé 
                   | [ ]5v    -| A |-               9[ ]~|   Pulsador Parada
                   | [ ]GND   -| R |-               8[ ] |   Pulsador Arranque
                   | [ ]GND   -| D |-                    |
                   | [ ]Vin   -| U |-               7[ ] |   
                   |          -| I |-               6[ ]~|   
Sensor Presión 1   | [ ]A0    -| N |-               5[ ]~|   Caudalímetro 4
Sensor Presión 2   | [ ]A1    -| O |-               4[ ] |   Caudalímetro 3
Sensor Presión 3   | [ ]A2     +---+           INT1/3[ ]~|   Caudalímetro 2
Sensor Presión 4   | [ ]A3                     INT0/2[ ] |   Caudalímetro 1
SDA del LCD        | [ ]A4/SDA  RST SCK MISO     TX>1[ ] |   
SCL del LCD        | [ ]A5/SCL  [ ] [ ] [ ]      RX<0[ ] |   
                   |            [ ] [ ] [ ]              |
                   |  UNO_R3    GND MOSI 5V  ____________/
                    \_______________________/

*/

  // LIBRERÍAS //
  #include <Wire.h> // Librería comunicación I2C
  #include <LiquidCrystal_I2C.h> // Librería LCD I2C
  #include "PinChangeInterrupt.h" 
  
  // DECLARACION DE LOS LCD //
  LiquidCrystal_I2C lcd1(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
  LiquidCrystal_I2C lcd2(0x26, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
    
  // DECLARACIÓN DE VARIABLES //
  // Caudalímetros
  const int sensor1 = 2; // Pin digital Caudalímetro 1
  const int sensor2 = 3; // Pin digital Caudalímetro 2
  const int sensor3 = 4; // Pin digital Caudalímetro 3
  const int sensor4 = 5; // Pin digital Caudalímetro 4
  int litros_minuto1; // Caudal 1 (lts/Min)
  int litros_minuto2; // Caudal 2 (lts/Min)
  int litros_minuto3; // Caudal 3 (lts/Min)
  int litros_minuto4; // Caudal 4 (lts/Min)
  volatile int pulsos1 = 0; // Acumulador de Pulsos Caudalímetro 1
  volatile int pulsos2 = 0; // Acumulador de Pulsos Caudalímetro 2
  volatile int pulsos3 = 0; // Acumulador de Pulsos Caudalímetro 3
  volatile int pulsos4 = 0; // Acumulador de Pulsos Caudalímetro 4
  unsigned long tiempoAnterior1 = 0; // Calcular el tiempo transcurrido C1
  unsigned long tiempoAnterior2 = 0; // Calcular el tiempo transcurrido C2
  unsigned long tiempoAnterior3 = 0; // Calcular el tiempo transcurrido C3
  unsigned long tiempoAnterior4 = 0; // Calcular el tiempo transcurrido C4
  unsigned long pulsos_Acumulados1 = 0; // Pulsos acumulados Caudalímetro 1
  unsigned long pulsos_Acumulados2 = 0; // Pulsos acumulados Caudalímetro 2
  unsigned long pulsos_Acumulados3 = 0; // Pulsos acumulados Caudalímetro 3
  unsigned long pulsos_Acumulados4 = 0; // Pulsos acumulados Caudalímetro 4
  int litros1; // Litros acumulados Caudalímetro 1
  int litros2; // Litros acumulados Caudalímetro 2
  int litros3; // Litros acumulados Caudalímetro 3
  int litros4; // Litros acumulados Caudalímetro 4
  
  // Sensores de Presión
  int sensorPin1 = A0; // Pin Analógico Sensor de Presión 1
  int sensorPin2 = A1; // Pin Analógico Sensor de Presión 2
  int sensorPin3 = A2; // Pin Analógico Sensor de Presión 3
  int sensorPin4 = A3; // Pin Analógico Sensor de Presión 4
  int sensorValue1; // Señal recibida Sensor 1
  int sensorValue2; // Señal recibida Sensor 2
  int sensorValue3; // Señal recibida Sensor 3
  int sensorValue4; // Señal recibida Sensor 4
  float value1; // Valor de la conversión Señal-Voltaje Sensor 1
  float value2; // Valor de la conversión Señal-Voltaje Sensor 2
  float value3; // Valor de la conversión Señal-Voltaje Sensor 3
  float value4; // Valor de la conversión Señal-Voltaje Sensor 4
  float P1; // Presión 1
  float P2; // Presión 2
  float P3; // Presión 3
  float P4; // Presión 4

  //Bomba
  int led = 13;
  int rele = 10;
  int pulsador1 = 8;
  int pulsador2 = 9;

  

  
  // FUNCIONES //
  // Rutina de servicio de la interrupción (ISR)
  void flujo1() 
  {
    pulsos1++; // Incrementa los pulsos
  }
  
  void flujo2() 
  {
    pulsos2++; // Incrementa los pulsos
  }

  void flujo3() 
  {
    pulsos3++; // Incrementa los pulsos
  }

  void flujo4() 
  {
    pulsos4++; // Incrementa los pulsos
  }
  
  // Convierte la señal en voltaje
  float fmap(float x, float in_min, float in_max, float out_min, float out_max)
  {
     return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }


   
  void setup()
  {
    lcd1.begin(20,4); // Inicializo el LCD 1
    lcd2.begin(20,4); // Inicializo el LCD 2
    pinMode(sensor1, INPUT_PULLUP); // Pin digital como entrada con conexión PULL-UP interna
    pinMode(sensor2, INPUT_PULLUP);
    pinMode(sensor3, INPUT_PULLUP);
    pinMode(sensor4, INPUT_PULLUP);
    interrupts(); // Habilito las interrupciones
    // Interrupción INT0, llama a la ISR llamada "flujo" en cada flanco de subida en el pin digital 2
    attachInterrupt(digitalPinToInterrupt(sensor1), flujo1, RISING);
    attachInterrupt(digitalPinToInterrupt(sensor2), flujo2, RISING);
    attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(sensor3), flujo3, RISING);
    attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(sensor4), flujo4, RISING);  
    tiempoAnterior1 = millis(); // Guardo el tiempo que tarda el ejecutarse el setup
    tiempoAnterior2 = millis(); // Guardo el tiempo que tarda el ejecutarse el setup
    tiempoAnterior3 = millis(); // Guardo el tiempo que tarda el ejecutarse el setup
    tiempoAnterior4 = millis(); // Guardo el tiempo que tarda el ejecutarse el setup
    pinMode(led,OUTPUT);
    pinMode(rele,OUTPUT);
    pinMode(pulsador1,INPUT);
    pinMode(pulsador2,INPUT);
    digitalWrite(rele, HIGH);
  }


   
  void loop ()
  {
    // Cada segundo calculamos e imprimimos el caudal y el número de litros consumidos
    
    // Caudalímetro 1
    if(millis() - tiempoAnterior1 > 1000)
    {
      tiempoAnterior1 = millis(); // Actualizo el nuevo tiempo
      pulsos_Acumulados1 += pulsos1; // Número de pulsos acumulados
      litros_minuto1 = (pulsos1 / 4.8); // Q = frecuencia / 4.8 (L/Min)
      litros1 = pulsos_Acumulados1*1.0/300; // Cada 300 pulsos son un litro
      pulsos1 = 0; // Pongo nuevamente el número de pulsos a cero 
    }

    // Caudalímetro 2
    if(millis() - tiempoAnterior2 > 1000)
    {
      tiempoAnterior2 = millis(); // Actualizo el nuevo tiempo
      pulsos_Acumulados2 += pulsos2; // Número de pulsos acumulados
      litros_minuto2 = (pulsos2 / 4.8); // Q = frecuencia / 4.8 (L/Min)
      litros2 = pulsos_Acumulados2*1.0/300; // Cada 300 pulsos son un litro
      pulsos2 = 0; // Pongo nuevamente el número de pulsos a cero
    }
    
    // Caudalímetro 3
    if(millis() - tiempoAnterior3 > 1000)
    {
      tiempoAnterior3 = millis(); // Actualizo el nuevo tiempo
      pulsos_Acumulados3 += pulsos3; // Número de pulsos acumulados
      litros_minuto3 = (pulsos3 / 4.8); // Q = frecuencia / 4.8 (L/Min)
      litros3 = pulsos_Acumulados3*1.0/300; // Cada 300 pulsos son un litro
      pulsos3 = 0; // Pongo nuevamente el número de pulsos a cero
    }

    // Caudalímetro 4
    if(millis() - tiempoAnterior4 > 1000)
    {
      tiempoAnterior4 = millis(); // Actualizo el nuevo tiempo
      pulsos_Acumulados4 += pulsos4; // Número de pulsos acumulados
      litros_minuto4 = (pulsos4 / 4.8); // Q = frecuencia / 4.8 (L/Min)
      litros4 = pulsos_Acumulados4*1.0/300; // Cada 300 pulsos son un litro
      pulsos4 = 0; // Pongo nuevamente el número de pulsos a cero
    }

    // Sensor de Presión 1
    sensorValue1 = analogRead(sensorPin1);
    value1 = fmap(sensorValue1, 0, 1023, 0.5, 4.5);
    P1 = 0.5+0.4*value1;
    
    // Sensor de Presión 2
    sensorValue2 = analogRead(sensorPin2);
    value2 = fmap(sensorValue2, 0, 1023, 0.5, 4.5);
    P2 = 0.5+0.4*value2;
    
    // Sensor de Presión 3
    sensorValue3 = analogRead(sensorPin3);
    value3 = fmap(sensorValue3, 0, 1023, 0.5, 4.5);
    P3 = 0.5+0.4*value3;
    
    // Sensor de Presión 4
    sensorValue4 = analogRead(sensorPin4);
    value4 = fmap(sensorValue4, 0, 1023, 0.5, 4.5);
    P4 = 0.5+0.4*value4;

    // Control de Bomba
    if(digitalRead(pulsador1)==HIGH)
    {
      digitalWrite(led, HIGH);
      digitalWrite(rele, LOW);
    }
    if(digitalRead(pulsador2)==HIGH)
   {
     digitalWrite(led, LOW);
     digitalWrite(rele, HIGH);
   }
    
    // Llamada a la función 
    LCD_2004();
  }

  
  // FUNCIÓN PARA MOSTRAR DATOS POR LCD // 
  void LCD_2004()
  {
    // Medidor 1:
    lcd1.clear();
    lcd1.setCursor(0,0);
    lcd1.print("Med.1: ");
    lcd1.setCursor(0,1);
    lcd1.print("Q:");
    lcd1.print(litros_minuto1, DEC); 
    lcd1.print(" L/m");
    lcd1.setCursor(0,2);
    lcd1.print("P:");
    lcd1.print(P1);    
    lcd1.print("bar");
    lcd1.setCursor(0,3);
    lcd1.print("V:");
    lcd1.print(litros1);    
    lcd1.print(" Lts");
    
    // Medidor 2:
    lcd1.setCursor(10,0);
    lcd1.print("Med.2: ");
    lcd1.setCursor(10,1);
    lcd1.print("Q:");
    lcd1.print(litros_minuto2, DEC); 
    lcd1.print(" L/m");
    lcd1.setCursor(10,2);
    lcd1.print("P:");
    lcd1.print(P2);    
    lcd1.print("bar");
    lcd1.setCursor(10,3);
    lcd1.print("V:");
    lcd1.print(litros2);    
    lcd1.print(" Lts");
    delay(1000);

    //Medidor 3:
    lcd2.clear();
    lcd2.setCursor(0,0);
    lcd2.print("Med.3: ");
    lcd2.setCursor(0,1);
    lcd2.print("Q:");
    lcd2.print(litros_minuto3, DEC); 
    lcd2.print(" L/m");
    lcd2.setCursor(0,2);
    lcd2.print("P:");
    lcd2.print(P3);    
    lcd2.print("bar");
    lcd2.setCursor(0,3);
    lcd2.print("V:");
    lcd2.print(litros3);    
    lcd2.print(" Lts");

    // Medidor 4:
    lcd2.setCursor(10,0);
    lcd2.print("Med.4: ");
    lcd2.setCursor(10,1);
    lcd2.print("Q:");
    lcd2.print(litros_minuto4, DEC); 
    lcd2.print(" L/m");
    lcd2.setCursor(10,2);
    lcd2.print("P:");
    lcd2.print(P4);    
    lcd2.print("bar");
    lcd2.setCursor(10,3);
    lcd2.print("V:");
    lcd2.print(litros4);    
    lcd2.print(" Lts");
    delay(1000);

  }
