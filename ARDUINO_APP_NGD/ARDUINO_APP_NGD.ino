//Importamos librerias necesarias.
#include <WiFi.h>
#include <BluetoothSerial.h>

char ssid[16]; // Almacenar el nombre de la red, maximo 16 caracteres.
char password[16]; // Almacenar la contraseña, maximo 16 caracteres.

int timeInit = 0;
int timeOut = 15000; // Tiempo maximo de espera (15 Seg).

unsigned long timeStart;
unsigned long timeAct;
int timeEnd = 3000;

int LED_ALARMA = 2; //Pin 2 como LED de alarma
int BUZZ_ALARMA = 4; //Pin 4 como alarma del gas.

int LED_APP = 23; //Pin 19 como LED para probar si hay conexión
int GAS = 18; //Pin 18 como controlador de la valvula solenoide.
int SYSTEM = 19; //Pin 21 como controlador de corriente del sensor.

int LECT_GAS = 35; //Pin 35 como lector del sensor de gas MQ-4.

int VAL_GAS; //Variable que almacenara las PPM en el momento de la fuga.

BluetoothSerial SerialBT; //Creando el objeto de la clase BluetoothSerial.
WiFiServer server(80);


void setup() {
  Serial.begin(9600); //Comunicacion serial a 9600 baudios.
  SerialBT.begin("ESP32"); // Nombre del dispositivo Bluetooth visible para otros dispositivos.

  // Esperar a que se establezca la conexión BT
  while (!SerialBT.available()) {
    delay(100);
  }

  // Leer los datos de la aplicación Android a través de BT
  String data = SerialBT.readStringUntil('\n'); // Leer la cadena de datos hasta el salto de línea
  Serial.println("Datos recibidos desde la aplicación Android: " + data); //Eliminar en fase final

  //Separar datos mediante una ","

  int commaIndex = data.indexOf(',');
  if (commaIndex != -1) {
    data.substring(0, commaIndex).toCharArray(ssid, sizeof(ssid)); // Almacenar el SSID en la variable 'ssid'
    data.substring(commaIndex + 1).toCharArray(password, sizeof(password)); // Almacenar la contraseña en la variable 'password'
    Serial.println("SSID: " + String(ssid)); //Comentar en fase final
    Serial.println("Contraseña: " + String(password)); //Comentar en fase final
  }

  timeInit = millis(); //Iniciar a contar 15 segundos

  // Conectar el ESP32 a la red WiFi utilizando los datos recibidos mediante BT desde Android
  WiFi.begin(ssid, password);

  Serial.print("Conectando a la red WiFi..."); //Comentar en fase final
  while (WiFi.status() != WL_CONNECTED) { //Verificar si esta desconectado

    if (millis() - timeInit >= timeOut) { //Verificar si no han pasado 15 segundos
      Serial.println("\n¡Error! No se pudo conectar a la red WiFi en 15 segundos."); //Eliminar en fase final
      timeInit = 0;
      break; //Detener el intento de conexión
    }
    delay(500);
    Serial.print("."); //Comentar en fase final
  }

  if (WiFi.status() == WL_CONNECTED) { //verificar si se conecta a wifi
    Serial.println("\nConexión WiFi establecida");
    Serial.println(WiFi.localIP()); //Comentar en fase final
    SerialBT.println(WiFi.localIP()); //Enviando la IP a la aplicación
    digitalWrite(SYSTEM, HIGH);
    server.begin();
  }

  //Configurar los pines como salida

  pinMode(LED_ALARMA, OUTPUT);
  pinMode(BUZZ_ALARMA, OUTPUT);
  pinMode(LED_APP, OUTPUT);
  pinMode(GAS, OUTPUT);
  pinMode(SYSTEM, OUTPUT);
  pinMode(LECT_GAS, ANALOG);

}

void loop() {

WiFiClient client = server.available();
  if (client) {
    if (client.connected()) {
      String requestData = client.readStringUntil('\r');
      if (requestData.indexOf("GET /onSystem") != -1) {
        digitalWrite(SYSTEM, HIGH);
      }
      //Sistema
      if (requestData.indexOf("GET /offSystem") != -1) {
        digitalWrite(SYSTEM, LOW);
      }
      if (requestData.indexOf("GET /onLed") != -1) {
        digitalWrite(LED_APP, HIGH);
      }
      //Led
      if (requestData.indexOf("GET /offLed") != -1) {
        digitalWrite(LED_APP, LOW);
      }
      //Electrovalvula
      if (requestData.indexOf("GET /onGas") != -1) {
        digitalWrite(GAS, HIGH);
      }
      if (requestData.indexOf("GET /offGas") != -1) {
        digitalWrite(GAS, LOW);
      }
      //Servidor
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<!DOCTYPE HTML>");
      delay(10);
      client.stop();
    }
  }

  //Detectando el gas y tomando acciones

  VAL_GAS = analogRead(LECT_GAS);

  Serial.println(analogRead(LECT_GAS));

  if (VAL_GAS > 1000 && VAL_GAS < 4096) {

    if(timeStart == 0){
      timeStart = millis();
      digitalWrite(LED_ALARMA, HIGH);
      digitalWrite(BUZZ_ALARMA, HIGH);
    }else{
      timeAct = millis();
      if((timeAct - timeStart) >= timeEnd && ((timeAct - timeStart) / 200) % 2 == 0){
        digitalWrite(LED_ALARMA, HIGH);
        delay(200);
        digitalWrite(LED_ALARMA, LOW);
        delay(200);
        digitalWrite(GAS, HIGH);
      }
    }
  } else {
    digitalWrite(LED_ALARMA, LOW);
    digitalWrite(BUZZ_ALARMA, LOW);
    digitalWrite(GAS, LOW);
    timeStart = 0;
  }

}