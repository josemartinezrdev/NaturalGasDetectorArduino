#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(9600); //Activando comunicacion serial.
  SerialBT.begin("ESP32"); //Le damos nombre al ESP32 para que sea visible como dispositivo BT.
}

void loop() {
  if (SerialBT.available()) { //Preguntar si hay datos en el serial BT
    String data = SerialBT.readStringUntil('\n'); //Leer la cadena hasta encontrar salto de linea.

    int commaIndex = data.indexOf(','); //Seperar los datos al encontrar una coma.
    if (commaIndex != -1) {
      String ssid = data.substring(0, commaIndex); //Obteniendo el SSID
      String password = data.substring(commaIndex + 1); //Obteniendo el PASSWORD

      Serial.println(ssid); //Mostrando el SSID
      Serial.println(password); //Mostrando el PASSWORD
      
    }
  }
}