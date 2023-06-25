#include "MQTT_ESP8266.h"
#include "LoRaE32_ESP8266_MQTT.h"

const char *topic = "dummyPub";

void setup()
{
  setupWiFi("OpenWrt", "12345678"); // establish a wifi connection
  MQTTMakeConnection();             // build a mqtt connnection with the mqtt broker
  lora_Set_Mode(HIGH, HIGH);        // Set E32 at Mode 3 (sleep mode M0=1 & M1=1)
  Serial.begin(9600);
  delay(500);
  subscribeTopic(topic);
  delay(1000);
  loraConfigSet();
}
void loop()
{
  stayAwake();  // Maintaining regularly connection to the server for incoming messages
  check(topic); // reconnect with the MQTT Broker, if the connection is broken or lost
  delay(1000);
}
