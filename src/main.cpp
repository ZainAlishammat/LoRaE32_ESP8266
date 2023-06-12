
#include <Arduino.h>
#include <stdio.h>
#include "func.h"
#include "loraE32.h"

#define QoS 0 // MQTT quality of service

const char *topic = "dummyPub";

char reciveMsg;
boolean subTopic;

void setup()
{
  // Set E32 at Mode 3 (sleep mode M0=1 & M1=1)
  loraPinModeSetup();
  lora_Set_Mode(HIGH, HIGH);
  Serial.begin(9600);
  delay(500);
  setupWiFi("OpenWrt", "12345678");
  MQTTMakeConnection();
  subTopic = subscribeTopic(topic);
  delay(1000);

  loraConfigSet();
}
void loop()
{
  stayAwake(); // Maintaining regularly connection to the server for incoming messages
  check(topic);

  delay(1000);
}
