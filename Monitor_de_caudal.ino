//Librerias
#include <SoftwareSerial.h>


SoftwareSerial MySerial(4, 3); //SIM800L Tx & Rx is connected to Arduino #3 & #2




int analogica=0;
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
int PinSensor = 2;    //Sensor conectado en el pin 2
float factor_conversion=4.8; //para convertir de frecuencia a caudal
int frecuencia; //obtenemos la Frecuencia de los pulsos en Hz
float caudal_L_m;
bool bandera;


void flujo1()
{ 
  NumPulsos++;  //incrementamos la variable de pulsos
} 

//---Función para obtener frecuencia de los pulsos--------
int ObtenerFrecuencia() 
{
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Desabilitamos las interrupciones
  frecuencia=NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}

void setup()
{
  Serial.begin(9600);//Velocidad del puerto serial de arduino
  MySerial.begin(9600);
  pinMode(PinSensor,INPUT);
  attachInterrupt(0,flujo1,RISING);
  Serial.println("Empezando");
  delay(2000);//Tiempo prudencial para el escudo inicie sesión de red con tu operador
  bandera= true;
  Serial.println("paso el setup"); 
} 



void loop(){
  Serial.println("Entro en el loop");
  Caudal();
  Serial.println("Paso el caudal");  
  comandosAT();//Llama a la función comandosAT
  Serial.println("Paso los comando At");  
  mostrarDatosSeriales();
  Serial.println("LLego al final del loop");
}


//Funcion de envio de datos
void comandosAT(){
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
  Serial.print("el caudal es" );
  Serial.println(caudal_L_m);
  String datos="GET https://api.thingspeak.com/update?api_key=LJO8PPE93VVYDQAO&field1=0" + String(caudal_L_m);
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

void Caudal()
{
        frecuencia= ObtenerFrecuencia(); 
        caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
  
}

//Apaga el sim y arduino por 20 minutos
