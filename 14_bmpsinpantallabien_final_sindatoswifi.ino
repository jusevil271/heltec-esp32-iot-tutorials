//====================================================
// ESTACION BMP280 IoT
// HELTEC WiFi LoRa 32 V3
// BLOQUE 1 - LIBRERIAS Y CONFIGURACION
//====================================================

#include "Arduino.h"

#include <WiFi.h>
#include <ThingSpeak.h>

#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//====================================================
// WIFI
//====================================================

const char* ssid = "";
const char* password = "";

//====================================================
// THINGSPEAK
//====================================================

unsigned long channelID = 3432751;

const char* writeAPIKey = "VA0EKME7SEHJJE6T";

WiFiClient client;

//====================================================
// BMP280
//====================================================

#define SDA_BMP 47
#define SCL_BMP 48

Adafruit_BMP280 bmp;

bool bmpOK = false;

//====================================================
// VARIABLES
//====================================================

float temperatura = 0;
float presion = 0;
float altitud = 0;

//====================================================
// TEMPORIZADORES
//====================================================

unsigned long tiempoLectura = 0;
unsigned long tiempoSerie = 0;
unsigned long tiempoThingSpeak = 0;

const unsigned long INTERVALO_LECTURA = 1000;
const unsigned long INTERVALO_SERIE = 5000;
const unsigned long INTERVALO_THINGSPEAK = 20000;

//====================================================
// CONECTAR WIFI
//====================================================

void conectarWiFi()
{
    WiFi.disconnect(true);

    delay(100);

    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid,password);

    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    while(WiFi.status()!=WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    Serial.println("WiFi conectado");

    Serial.print("IP: ");

    Serial.println(WiFi.localIP());

    ThingSpeak.begin(client);
}

//====================================================
// ESCANER I2C
//====================================================

void escanerI2C()
{
    Serial.println();

    Serial.println("Buscando dispositivos I2C...");

    byte encontrados = 0;

    for(byte i=1;i<127;i++)
    {
        Wire.beginTransmission(i);

        if(Wire.endTransmission()==0)
        {
            Serial.print("Encontrado: 0x");

            if(i<16)
                Serial.print("0");

            Serial.println(i,HEX);

            encontrados++;
        }
    }

    if(encontrados==0)
    {
        Serial.println("NO HAY DISPOSITIVOS I2C");
    }

    Serial.println();
}

//====================================================
// SETUP
//====================================================

void setup()
{
    Serial.begin(115200);

    delay(2000);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("ESTACION BMP280 IoT");
    Serial.println("==========================================");

    //------------------------------------
    // BUS I2C
    //------------------------------------

    Wire.begin(SDA_BMP,SCL_BMP);

    delay(200);

    escanerI2C();

    //------------------------------------
    // BMP280
    //------------------------------------

    if(bmp.begin(0x76))
    {
        bmp.setSampling(
        Adafruit_BMP280::MODE_NORMAL,
        Adafruit_BMP280::SAMPLING_X2,
        Adafruit_BMP280::SAMPLING_X16,
        Adafruit_BMP280::FILTER_X16,
        Adafruit_BMP280::STANDBY_MS_500
    );

    bmpOK = true;

    Serial.println("BMP280 OK");
    }
    else
    {
        bmpOK = false;

        Serial.println("ERROR BMP280");
    }

    //------------------------------------
    // WIFI
    //------------------------------------

    conectarWiFi();

    //------------------------------------
    // FIN
    //------------------------------------

    Serial.println();
    Serial.println("Sistema preparado.");
    Serial.println();
}

//====================================================
// LEER BMP280
//====================================================

void leerSensor()
{
    if(!bmpOK)
        return;

    temperatura = bmp.readTemperature();

    presion = bmp.readPressure() / 100.0F;

    altitud = bmp.readAltitude(1013.25);
}

//====================================================
// MONITOR SERIE
//====================================================

void mostrarMonitorSerie()
{
    Serial.println();
    Serial.println("==========================================");

    if(bmpOK)
    {
        Serial.print("Temperatura : ");
        Serial.print(temperatura,1);
        Serial.println(" C");

        Serial.print("Presion     : ");
        Serial.print(presion,1);
        Serial.println(" hPa");

        Serial.print("Altitud     : ");
        Serial.print(altitud,1);
        Serial.println(" m");
    }
    else
    {
        Serial.println("BMP280 NO DISPONIBLE");
    }

    Serial.print("WiFi        : ");

    if(WiFi.status()==WL_CONNECTED)
        Serial.println("OK");
    else
        Serial.println("DESCONECTADO");

    Serial.println("==========================================");
}

//====================================================
// ENVIAR A THINGSPEAK
//====================================================

bool enviarThingSpeak()
{
    if(!bmpOK)
        return false;

    ThingSpeak.setField(1, temperatura);
    ThingSpeak.setField(2, presion);
    ThingSpeak.setField(3, altitud);
    ThingSpeak.setField(4, 1);

    int codigo = ThingSpeak.writeFields(channelID, writeAPIKey);

    if(codigo==200)
    {
        Serial.println("ThingSpeak OK");
        return true;
    }

    Serial.print("Error ThingSpeak: ");

    Serial.println(codigo);

    return false;
}

//====================================================
// LOOP PRINCIPAL
//====================================================

void loop()
{
    //------------------------------------------
    // Reconectar WiFi
    //------------------------------------------

    if(WiFi.status()!=WL_CONNECTED)
    {
        Serial.println("Reconectando WiFi...");

        conectarWiFi();
    }

    //------------------------------------------
    // Lectura cada segundo
    //------------------------------------------

    if(millis()-tiempoLectura>=INTERVALO_LECTURA)
    {
        leerSensor();

        tiempoLectura=millis();
    }

    //------------------------------------------
    // Monitor serie cada 5 segundos
    //------------------------------------------

    if(millis()-tiempoSerie>=INTERVALO_SERIE)
    {
        mostrarMonitorSerie();

        tiempoSerie=millis();
    }

    //------------------------------------------
    // ThingSpeak cada 20 segundos
    //------------------------------------------

    if(millis()-tiempoThingSpeak>=INTERVALO_THINGSPEAK)
    {
        enviarThingSpeak();

        tiempoThingSpeak=millis();
    }
}