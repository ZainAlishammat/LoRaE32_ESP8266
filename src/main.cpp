#include "MQTT_ESP8266.h"
#include "LoRaE32_ESP8266_MQTT.h"

const char *MQTT_TOPICS[] = {"dummyPub", "temp"};
uint8_t topicsSize = sizeof(MQTT_TOPICS) / sizeof(MQTT_TOPICS[0]);

receivedDataStruct RD_;

void setup()
{
  Serial.begin(115200);

  MQTTMakeConnection(); // build a mqtt connnection with the mqtt broker

  delay(500);
  // subscribeTopic(MQTT_TOPICS, topicsSize);

  /* Set LoRa E32 mode
      MODE_0_NORMAL
      MODE_1_WAKE_UP
      MODE_2_POWER_SAVING
      MODE_3_SLEEP
  */
  lora_Set_Mode(MODE_3_SLEEP);
  loraConfigSet();
  LoRa_recieveMsg(RD_);
}
void loop()
{
  MQTT_stayAwake();                    // Maintaining regularly connection to the server for incoming messages
  MQTT_check(MQTT_TOPICS, topicsSize); // reconnect with the MQTT Broker, if the connection is broken or lost
  delay(1000);
}
