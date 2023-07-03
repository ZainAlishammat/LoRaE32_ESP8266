#include "MQTT_ESP8266.h"
#include "LoRaE32_ESP8266_MQTT.h"

const char *MQTT_TOPICS[] = {"dummyPub", "temp"};
uint8_t topicsSize = sizeof(MQTT_TOPICS) / sizeof(MQTT_TOPICS[0]);

const char *topicName = {"Day"};
char publishTopic[] = {"Monday is terrible!"};
const uint16_t sizeT = sizeof(publishTopic) / sizeof(publishTopic[0]);

receivedDataStruct RD_;

void setup()
{
  Serial.begin(115200);

  MQTTMakeConnection(); // build a mqtt connnection with the mqtt broker
  delay(500);
  subscribeTopic(MQTT_TOPICS, topicsSize);

  lora_Set_Mode(MODE_0_NORMAL);
  loraConfigSet();
  LoRa_recieveMsg(RD_);
}
void loop()
{
  publisheMsg(topicName, publishTopic, sizeT, true);
  MQTT_stayAwake();                    // Maintaining regularly connection to the server for incoming messages
  MQTT_check(MQTT_TOPICS, topicsSize); // reconnect with the MQTT Broker, if the connection is broken or lost
  // Serial.println(RD_.rc_Data);
  delay(1000);
}
