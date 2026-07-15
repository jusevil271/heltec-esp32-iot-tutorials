#include "Arduino.h"
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include "HT_SSD1306Wire.h"

//====================================================
// WIFI
//====================================================
const char* ssid = "Avatel_4C";
const char* password = "gs5cp88R";

//====================================================
// THINGSPEAK
//====================================================
unsigned long channelID = 3427516;

const char* writeAPIKey = "3V4SYV120EWIIEPQ";

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

int valor = 20;

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
}

//====================================================

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.print("Enviando: ");
    Serial.println(valor);

    int codigo = ThingSpeak.writeField(
                    channelID,
                    1,
                    valor,
                    writeAPIKey);

    if(codigo == 200)
    {
      Serial.println("Dato enviado correctamente");

      factory_display.clear();
      factory_display.drawString(0,0,"ThingSpeak OK");
      factory_display.drawString(0,20,"Valor:");
      factory_display.drawString(45,20,String(valor));
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

    valor++;

    if(valor>40)
      valor=20;
  }
  else
  {
    Serial.println("WiFi desconectado");
  }

  delay(20000);
}