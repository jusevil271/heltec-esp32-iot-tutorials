#include "Arduino.h"

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
// Pantallas OLED
//====================================================

byte pantalla = 0;

const byte NUM_PANTALLAS = 5;

unsigned long ultimoCambioPantalla = 0;

const unsigned long TIEMPO_PANTALLA = 3000;

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

  switch (pantalla)
  {

    //--------------------------------------------------
    // Pantalla 0
    //--------------------------------------------------

    case 0:

      factory_display.drawString(0,0,"GPS OK");

      factory_display.drawString(
        0,
        18,
        "Sat: " + String(gps.satellites.value())
      );

      factory_display.drawString(
        70,
        18,
        "Alt:"
        + String(gps.altitude.meters(),0)
      );

      factory_display.drawString(
        0,
        40,
        "RX: " + String(bytesGPS)
      );

      break;

    //--------------------------------------------------
    // Pantalla 1
    //--------------------------------------------------

    case 1:

      factory_display.drawString(0,0,"LAT");

      factory_display.drawString(
        0,
        18,
        String(gps.location.lat(),6)
      );

      factory_display.drawString(0,40,"LON");

      factory_display.drawString(
        0,
        52,
        String(gps.location.lng(),6)
      );

      break;

    //--------------------------------------------------
    // Pantalla 2
    //--------------------------------------------------

    case 2:

      factory_display.drawString(0,0,"MOVIMIENTO");

      factory_display.drawString(
        0,
        18,
        "Vel: "
        + String(gps.speed.kmph(),1)
        + " km/h"
      );

      factory_display.drawString(
        0,
        34,
        "Curso: "
        + String(gps.course.deg(),0)
      );

      factory_display.drawString(
        0,
        50,
        "HDOP: "
        + String(gps.hdop.hdop(),1)
      );

      break;

    //--------------------------------------------------
    // Pantalla 3
    //--------------------------------------------------

    case 3:

      factory_display.drawString(0,0,"HORA UTC");

      if(gps.time.isValid())
      {
        char hora[12];

        sprintf(
          hora,
          "%02d:%02d:%02d",
          gps.time.hour(),
          gps.time.minute(),
          gps.time.second()
        );

        factory_display.drawString(0,20,hora);
      }

      if(gps.date.isValid())
      {
        char fecha[20];

        sprintf(
          fecha,
          "%02d/%02d/%04d",
          gps.date.day(),
          gps.date.month(),
          gps.date.year()
        );

        factory_display.drawString(0,42,fecha);
      }

      break;

    //--------------------------------------------------
    // Pantalla 4
    //--------------------------------------------------

    case 4:

      factory_display.drawString(0,0,"DIAGNOSTICO");

      factory_display.drawString(
        0,
        18,
        "Chars: "
        + String(gps.charsProcessed())
      );

      factory_display.drawString(
        0,
        34,
        "Err: "
        + String(gps.failedChecksum())
      );

      factory_display.drawString(
        0,
        50,
        "Fix: "
        + String(gps.sentencesWithFix())
      );

      break;

  }

  factory_display.display();
}

//====================================================
// Rotación automática de pantallas
//====================================================

void cambiarPantalla()
{
  if (millis() - ultimoCambioPantalla >= TIEMPO_PANTALLA)
  {
    ultimoCambioPantalla = millis();

    pantalla++;

    if (pantalla >= NUM_PANTALLAS)
      pantalla = 0;
  }
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

  cambiarPantalla();

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