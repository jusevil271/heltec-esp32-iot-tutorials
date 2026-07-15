#include "Arduino.h"
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include "HT_SSD1306Wire.h"

//====================================================
// WIFI
//====================================================
const char* ssid = "";
const char* password = "";

//====================================================
// THINGSPEAK
//====================================================
unsigned long channelID = 3427516;
const char* writeAPIKey = "3V4SYV120EWIIEPQ";

//====================================================
// SENSOR DE LUZ
//====================================================
const int sensorLuz = 2;   // GPIO2

//====================================================
// OLED
//====================================================
SSD1306Wire factory_display(
  0x3c,
  500000,
  SDA_OLED,
  SCL_OLED,
  GEOMETRY_128_64,
  RST_OLED
);

//====================================================

WiFiClient client;

//====================================================

void VextON()
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

//====================================================

void conectarWiFi()
{
  WiFi.disconnect(true);
  delay(100);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  factory_display.clear();
  factory_display.drawString(0,0,"Conectando...");
  factory_display.display();

  Serial.print("Conectando a ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  factory_display.clear();
  factory_display.drawString(0,0,"WiFi OK");
  factory_display.drawString(0,15,WiFi.localIP().toString());
  factory_display.display();
}

//====================================================

void setup()
{
  Serial.begin(115200);

  VextON();
  delay(100);

  factory_display.init();
  factory_display.clear();
  factory_display.display();

  conectarWiFi();

  ThingSpeak.begin(client);

  pinMode(sensorLuz, INPUT);
}

//====================================================

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    // Leer el sensor de luz
    int luz = analogRead(sensorLuz);

    Serial.println();
    Serial.print("Nivel de luz: ");
    Serial.println(luz);

    int codigo = ThingSpeak.writeField(
                    channelID,
                    1,
                    luz,
                    writeAPIKey);

    if(codigo == 200)
    {
      Serial.println("Dato enviado correctamente");

      factory_display.clear();
      factory_display.drawString(0,0,"ThingSpeak OK");
      factory_display.drawString(0,18,"Luz:");
      factory_display.drawString(35,18,String(luz));

      // Barra de progreso (0-4095 -> 0-128 píxeles)
      int anchoBarra = map(luz, 0, 4095, 0, 128);
      factory_display.drawRect(0, 45, 128, 12);
      factory_display.fillRect(0, 45, anchoBarra, 12);

      factory_display.display();
    }
    else
    {
      Serial.print("Error: ");
      Serial.println(codigo);

      factory_display.clear();
      factory_display.drawString(0,0,"Error");
      factory_display.drawString(0,20,String(codigo));
      factory_display.display();
    }
  }
  else
  {
    Serial.println("WiFi desconectado");
  }

  // ThingSpeak gratuito: mínimo 15 segundos
  delay(20000);
}