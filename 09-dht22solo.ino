#include "Arduino.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"
#include <DHT.h>

//======================================
// OLED
//======================================

SSD1306Wire factory_display(
  0x3c,
  500000,
  SDA_OLED,
  SCL_OLED,
  GEOMETRY_128_64,
  RST_OLED
);

//======================================

#define DHTPIN 1
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//======================================

void VextON()
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

//======================================

void setup()
{
  Serial.begin(115200);

  VextON();

  delay(100);

  factory_display.init();
  factory_display.clear();

  factory_display.drawString(0,0,"AM2302");
  factory_display.drawString(0,18,"Iniciando...");
  factory_display.display();

  dht.begin();
}

//======================================

void loop()
{
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  if (isnan(temperatura) || isnan(humedad))
  {
    Serial.println("Error leyendo AM2302");

    factory_display.clear();
    factory_display.drawString(0,0,"ERROR");
    factory_display.drawString(0,20,"Sensor");
    factory_display.display();

    delay(2000);
    return;
  }

  Serial.println("-----------------------");

  Serial.print("Temperatura: ");
  Serial.print(temperatura,1);
  Serial.println(" C");

  Serial.print("Humedad: ");
  Serial.print(humedad,1);
  Serial.println(" %");

  factory_display.clear();

  factory_display.drawString(0,0,"AM2302 OK");

  factory_display.drawString(
    0,
    18,
    "Temp: " + String(temperatura,1) + " C"
  );

  factory_display.drawString(
    0,
    36,
    "Hum : " + String(humedad,1) + " %"
  );

  factory_display.display();

  delay(2000);
}