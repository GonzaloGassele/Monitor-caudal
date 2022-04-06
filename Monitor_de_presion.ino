  //     Librerias
  #include <SoftwareSerial.h>
  #include <LowPower.h>
  #include <queue>

  SoftwareSerial MySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2


  int PinSensor = 2;    //Sensor conectado en el pin 2
  int valor; 
  float evalor; 
  float fvalor;
  float psi;
  float kg;
  int hora;
  String Bateria;
  String cbc;
  queue<float> cola;



  void setup()
  {
    Serial.begin(9600);//Velocidad del puerto serial de arduino
    MySerial.begin(9600);// Velocidad del modulo SIM
    Serial.println("Empezando");
    hora = 0;//
  } 



  void loop(){
      kg= presion();// toma valor de la presion 
      enviar_presion(kg);//Envia presión, sino puede enviar, guarda el dato
      enviar_bateria()
      delay(5000);
      dormir(); 
  }


  //Funciones de presión


  //mapea el Pin del sensor y transforma el valor el voltaje
  float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
  {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

  //convierte el valor de voltaje en kg/cm2
  float presion() {
    valor = analogRead(A0);
    evalor = mapfloat(valor, 0, 1023, 0, 5); //mapeamos a voltios *10 para mayor resolucion
    psi= mapfloat(evalor, 0.5, 4.5, 0, 150);
    kg= (psi*0.070307);
    return kg
  }

  //Funcion de envio de datos
  void enviar_presion(kg){
    resp= send_data(kg);
    comprobar_envio()
    if no envio
      Guardar dato
    else
      for d in cola
        send_data(d);
        borrar_datos_en_cola
  } 

  void enviar_bateria(){
    Bateria= dato_bateria();
    Serial.print("la bateria es " );
    Serial.println(Bateria);
    send_data(Bateria);
  } 
    enviar();
    comprobar_envio();
    if no envio
    guardar_datos();

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

  //Envió de comando AT

  void ComandoAT(String at)
  {
    MySerial.println("AT");
    delay(1000);
    mostrarDatosSeriales();
  }


  //Genera dato de bateria

  int dato_bateria()
  {
    while (MySerial.available())
    {
      cbc=MySerial.readString();//Forward what MySerial received to Software MySerial Port
    }
    Bateria= cbc.substring(17,20);
    return Bateria;
  }

  //Envio de datos al servidor

  void send_data(data){
    ComandoAT("AT");
    ComandoAT("AT+CSCLK=0");
    ComandoAT("AT+CIPSPRT=0");//Establece un indicador '>' al enviar datos
    ComandoAT("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//Indicamos el tipo de conexión, url o dirección IP y puerto al que realizamos la conexión;
    ComandoAT("AT+CIPSEND");//ENVÍA DATOS A TRAVÉS DE una CONEXIÓN TCP O UDP
    Serial.print("la presion es " );
    Serial.println(data);
    String datos="GET https://api.thingspeak.com/update?api_key=TWYV324S2A05AOVR&field1=0" + String(data);
    ComandoAT(datos);//Envía datos al servidor remoto
    ComandoAT(char)26);
    ComandoAT("AT+CIPSHUT");//Cierra la conexión(Desactiva el contexto GPRS PDP);
    ComandoAT("AT+CSCLK=2");
  } 

void actualizar_hora()
{
  hora=0;
  }

hora= hora +1;
    if (hora==96){
      MySerial.println("AT");
      delay(1000);
      mostrarDatosSeriales();
      MySerial.println("AT+CBC");
      delay(3000);
      dato_bateria();
      enviar_bateria();
      delay(3000);
      actualizar_hora();
    }
    for (int i = 0 ;  i  <  100 ; i++)
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
