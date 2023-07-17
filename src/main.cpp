#include "LoRaE32_ESP8266_MQTT.h"

char buff[100];
void setup()
{

  Serial.begin(115200);

  loraConfigSet(MODE_0_NORMAL);
  delay(100);
}
void loop()
{
  loraSendString("NOK");
  delay(2000);
}
