#include "Arduino.h"
#include "WiFi.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

//========================
// CONFIGURACIÓN WIFI
//========================
const char* ssid = "";
const char* password = "";

//========================
// OLED
//========================
SSD1306Wire factory_display(
  0x3c,
  500000,
  SDA_OLED,
  SCL_OLED,
  GEOMETRY_128_64,
  RST_OLED
);

//========================
// Encender OLED
//========================
void VextON()
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

//========================
// Conexión WiFi
//========================
void WIFISetUp()
{
  WiFi.disconnect(true);
  delay(100);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  WiFi.begin(ssid, password);

  factory_display.clear();
  factory_display.drawString(0,0,"Conectando WiFi...");
  factory_display.display();

  Serial.println();
  Serial.println("===========================");
  Serial.println("HELTEC ESP32 WiFi Kit 32");
  Serial.println("===========================");
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println();

  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  factory_display.clear();

  factory_display.drawString(0,0,"WiFi OK");

  factory_display.drawString(
      0,
      15,
      WiFi.localIP().toString()
  );

  factory_display.drawString(
      0,
      35,
      "RSSI: " + String(WiFi.RSSI()) + " dBm"
  );

  factory_display.display();
}

//========================
// SETUP
//========================
void setup()
{
  Serial.begin(115200);

  VextON();

  delay(100);

  factory_display.init();

  factory_display.clear();

  factory_display.display();

  WIFISetUp();
}

//========================
// LOOP
//========================
void loop()
{

}