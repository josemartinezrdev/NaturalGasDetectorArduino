//Importamos librerias necesarias.
#include <WiFi.h>
#include <BluetoothSerial.h>

char ssid[16]; // Almacenar el nombre de la red, maximo 16 caracteres.
char password[16]; // Almacenar la contraseña, maximo 16 caracteres.
int timeInit = 0;
int timeOut = 15000; // Tiempo maximo de espera (15 Seg).

int led = 2;

BluetoothSerial SerialBT; //Creando el objeto de la clase BluetoothSerial.

void setup() {
  Serial.begin(9600); //Comunicacion serial a 9600 baudios.
  SerialBT.begin("ESP32"); // Nombre del dispositivo Bluetooth visible para otros dispositivos.

  pinMode(led, OUTPUT);

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
    Serial.println("SSID: " + String(ssid)); //Eliminar en fase final
    Serial.println("Contraseña: " + String(password)); //Eliminar en fase final
  }

  timeInit = millis(); //Iniciar a contar 15 segundos

  // Conectar el ESP32 a la red WiFi utilizando los datos recibidos mediante BT desde Android
  WiFi.begin(ssid, password);

  Serial.print("Conectando a la red WiFi..."); //Eliminar en fase final
  while (WiFi.status() != WL_CONNECTED) { //Verificar si esta desconectado

    if (millis() - timeInit >= timeOut) { //Verificar si no han pasado 15 segundos
      Serial.println("\n¡Error! No se pudo conectar a la red WiFi en 15 segundos."); //Eliminar en fase final
      break; //Detener el intento de conexión
    }
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) { //verificar si se conecta a wifi
    Serial.println("\nConexión WiFi establecida");
    Serial.println(WiFi.localIP()); //Eliminar en fase final
    digitalWrite(led, HIGH); //Eliminar en fase final

    SerialBT.println(WiFi.localIP()); //Enviando la IP a la aplicación

  }
}

void loop() {
  
}
