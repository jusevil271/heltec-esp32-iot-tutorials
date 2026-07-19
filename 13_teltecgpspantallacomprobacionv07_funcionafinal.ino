e<#include "Arduino.h"

#include "HT_SSD1306Wire.h"
#include <HT_TinyGPS++.h>

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
// GPS
//====================================================

TinyGPSPlus gps;

// RX de la Heltec <- TX del GPS
#define GPS_RX 39

// TX de la Heltec -> RX del GPS
#define GPS_TX 40

unsigned long bytesGPS = 0;

//====================================================

void VextON()
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

//====================================================

void mostrarInicio()
{
  factory_display.clear();

  factory_display.drawString(0,0,"HELTEC + GPS");
  factory_display.drawString(0,18,"Inicializando...");
  factory_display.display();
}

//====================================================

void mostrarEsperando()
{
  factory_display.clear();

  factory_display.drawString(0,0,"GPS");
  factory_display.drawString(0,18,"Esperando datos...");
  factory_display.drawString(0,36,"RX: " + String(bytesGPS));

  factory_display.display();
}

//====================================================

void mostrarGPS()
{
  factory_display.clear();

  factory_display.drawString(0,0,"GPS OK");

  factory_display.drawString(
    0,
    14,
    String(gps.location.lat(),5)
  );

  factory_display.drawString(
    0,
    28,
    String(gps.location.lng(),5)
  );

  factory_display.drawString(
    0,
    42,
    "Sat: " + String(gps.satellites.value())
  );

  factory_display.drawString(
    70,
    42,
    "Alt:" + String(gps.altitude.meters(),0)
  );

  factory_display.display();
}

//====================================================

void setup()
{
  Serial.begin(115200);

  VextON();

  delay(100);

  factory_display.init();

  mostrarInicio();

  Serial.println();
  Serial.println("====================================");
  Serial.println(" HELTEC V3 + GPS NEO-6M");
  Serial.println("====================================");

  Serial.println("Iniciando UART2...");

  Serial2.begin(
    9600,
    SERIAL_8N1,
    GPS_RX,
    GPS_TX
  );

  Serial.println("UART2 iniciada.");
  Serial.println("Esperando tramas NMEA...");
}

//====================================================

void loop()
{
  while (Serial2.available())
  {
    char c = Serial2.read();

    bytesGPS++;

    Serial.write(c);

    gps.encode(c);
  }

  static unsigned long ultimo = 0;

  if (millis() - ultimo > 3000)
  {
    ultimo = millis();

    Serial.println();
    Serial.println("------------------------------------");
    Serial.print("Bytes recibidos: ");
    Serial.println(bytesGPS);

    if (gps.location.isValid())
    {
      Serial.println("GPS OK");

      Serial.print("Latitud : ");
      Serial.println(gps.location.lat(),6);

      Serial.print("Longitud: ");
      Serial.println(gps.location.lng(),6);

      Serial.print("Satelites: ");
      Serial.println(gps.satellites.value());

      Serial.print("Altitud : ");
      Serial.println(gps.altitude.meters());

      mostrarGPS();
    }
    else
    {
      Serial.println("Aun sin posicion...");
      Serial.println("Esperando satelites...");

      mostrarEsperando();
    }
  }
}