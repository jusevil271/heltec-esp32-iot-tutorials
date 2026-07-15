#include <Wire.h>
#include "HT_SSD1306Wire.h"

static SSD1306Wire display(
  0x3c,
  500000,
  SDA_OLED,
  SCL_OLED,
  GEOMETRY_128_64,
  RST_OLED
);

void VextON()
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void setup()
{
  Serial.begin(115200);

  VextON();
  delay(100);

  display.init();
  display.clear();

  display.setTextAlignment(TEXT_ALIGN_CENTER);

  display.setFont(ArialMT_Plain_24);
  display.drawString(64, 18, "Hola");

  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 46, "Montesteam");

  display.display();
}

void loop()
{
  // No hacer nada
}