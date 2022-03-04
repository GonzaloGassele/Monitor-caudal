//Librerias
#include <SoftwareSerial.h>


SoftwareSerial MySerial(4, 3); //SIM800L Tx & Rx is connected to Arduino #3 & #2



  // Sensores de Presión  
  int sensorPres= A0; // Pin Analógico Sensor de Presión
  int sensorValue; // Señal recibida del Sensor
  float value; // Valor de la conversión Señal-Voltaje 
  float psi; //Valor de la presiòn en psi del sensor
  float Pkg; // Presión en kg/cm2

 // Convierte la señal en voltaje
  float fmap(float x, float in_min, float in_max, float out_min, float out_max)
  {
     return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }



void setup()
{
  Serial.begin(9600);//Velocidad del puerto serial de arduino
  MySerial.begin(9600);
  pinMode(PinSensor,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PinSensor),flujo1,RISING);
  tiempoAnterior1 = millis(); // Guardo el tiempo que tarda el ejecutarse el setup
  Serial.println("Empezando");
} 



void loop(){
  Presion();  
  comandosAT();//Llama a la función comandosAT 
  mostrarDatosSeriales();
}


//Funcion de envio de datos
void comandosAT(){
  Serial.println("Entro al comando at");
  MySerial.println("AT+CIPSTATUS");//Consultar el estado actual de la conexión
  delay(2000);
  MySerial.println("AT+CIPMUX=0");//comando configura el dispositivo para una conexión IP única o múltiple 0=única
  delay(3000);
  mostrarDatosSeriales();
  delay(1000);
  mostrarDatosSeriales();
  MySerial.println("AT+CIICR");//REALIZAR UNA CONEXIÓN INALÁMBRICA CON GPRS O CSD
  delay(3000);
  mostrarDatosSeriales();
  MySerial.println("AT+CIFSR");// Obtenemos nuestra IP local
  delay(2000);
  mostrarDatosSeriales();
  MySerial.println("AT+CIPSPRT=0");//Establece un indicador '>' al enviar datos
  delay(3000);
  mostrarDatosSeriales();
  MySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//Indicamos el tipo de conexión, url o dirección IP y puerto al que realizamos la conexión
  delay(6000);
  mostrarDatosSeriales();
  MySerial.println("AT+CIPSEND");//ENVÍA DATOS A TRAVÉS DE una CONEXIÓN TCP O UDP
  delay(4000);
  mostrarDatosSeriales();
  Serial.print("el caudal es " );
  Serial.println(litros_minuto1);
  String datos="GET https://api.thingspeak.com/update?api_key=LJO8PPE93VVYDQAO&field1=0" + String(Pkg);
  MySerial.println(datos);//Envía datos al servidor remoto
  delay(4000);
  mostrarDatosSeriales();
  MySerial.println((char)26);
  delay(5000);//Ahora esperaremos una respuesta pero esto va a depender de las condiones de la red y este valor quizá debamos modificarlo dependiendo de las condiciones de la red
  MySerial.println();
  mostrarDatosSeriales();
  MySerial.println("AT+CIPSHUT");//Cierra la conexión(Desactiva el contexto GPRS PDP)
  delay(5000);
  mostrarDatosSeriales();
  Serial.print("Salio del comando at");
} 

//Muestra los datos que va entregando el sim900

void mostrarDatosSeriales()
{
  delay(500);
  while (Serial.available())
  {
  MySerial.write(Serial.read());//Forward what MySerial received to Software MySerial Port
  }
  while (MySerial.available())
  {
  Serial.write(MySerial.read());//Forward what MySerial received to Software MySerial Port
  }
}

// Calcula el caudal


void Presion()
{
   // Sensor de Presión 1
    sensorValue = analogRead(sensorPinPres);
    value = fmap(sensorValue, 0, 1023, 0, 5);
    psi = fmap(value, 1.04, 5, 0, 150);
    Pkg = (psi*0.070307);
  }

//Apaga el sim y arduino por 20 minutos
