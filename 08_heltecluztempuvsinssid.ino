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
// SENSORES
//====================================================
const int sensorLuz = 2;           // GPIO2
const int sensorTemperatura = 1;   // GPIO1 (LM35)
const int sensorUV = 3;            // GPIO3 (GUVA-S12SD)

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
  pinMode(sensorTemperatura, INPUT);
  pinMode(sensorUV, INPUT);
}

//====================================================

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    //====================================
    // SENSOR DE LUZ
    //====================================
    int luz = analogRead(sensorLuz);

    //====================================
    // SENSOR LM35
    //====================================
    int lecturaLM35 = analogRead(sensorTemperatura);

    float voltaje = lecturaLM35 * (3.3 / 4095.0);
    float temperatura = voltaje * 100.0;

    //====================================
    // SENSOR UV
    //====================================
    int lecturaUV = analogRead(sensorUV);

    float voltajeUV = lecturaUV * (3.3 / 4095.0);

    //====================================
    // MONITOR SERIE
    //====================================

    Serial.println("-------------------------------------");

    Serial.print("Temperatura: ");
    Serial.print(temperatura,1);
    Serial.println(" °C");

    Serial.print("Nivel de luz: ");
    Serial.println(luz);

    Serial.print("Sensor UV: ");
    Serial.println(lecturaUV);

    Serial.print("Voltaje UV: ");
    Serial.print(voltajeUV,3);
    Serial.println(" V");

    //====================================
    // ENVIAR A THINGSPEAK
    //====================================

    ThingSpeak.setField(1, temperatura);
    ThingSpeak.setField(2, luz);
    ThingSpeak.setField(3, lecturaUV);

    int codigo = ThingSpeak.writeFields(channelID, writeAPIKey);

    //====================================
    // OLED
    //====================================

    factory_display.clear();

    if(codigo == 200)
    {
      Serial.println("Datos enviados correctamente");
      factory_display.drawString(0,0,"ThingSpeak OK");
    }
    else
    {
      Serial.print("Error: ");
      Serial.println(codigo);

      factory_display.drawString(0,0,"Error");
      factory_display.drawString(50,0,String(codigo));
    }

    factory_display.drawString(0,14,"Temp:");
    factory_display.drawString(42,14,String(temperatura,1)+" C");

    factory_display.drawString(0,28,"Luz:");
    factory_display.drawString(42,28,String(luz));

    factory_display.drawString(0,42,"UV:");
    factory_display.drawString(42,42,String(lecturaUV));

    // Barra de luz
    int anchoBarra = map(luz,0,4095,0,128);

    factory_display.drawRect(0,56,128,8);
    factory_display.fillRect(0,56,anchoBarra,8);

    factory_display.display();
  }
  else
  {
    Serial.println("WiFi desconectado");
  }

  // ThingSpeak gratuito: mínimo 15 segundos
  delay(20000);
}