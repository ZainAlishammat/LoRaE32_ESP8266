#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <stdio.h>

void setupWiFi(const char *SSID, const char *password);
void MQTTMakeConnection();
void stayAwake();
boolean publishMsg(const char *topic, const char *payload);
boolean publishNum(const char *topic, const uint8_t *payload, unsigned int length);
boolean subscribeTopic(const char *topic);
boolean unsubscribeTopic(const char *topic);
void __subCallback(char *topic, byte *payload, unsigned int length);
// void __handleInterrupt_UART_Rx();
// void lora_Inspector(void);
// void lora_Set_Mode(uint8_t M0, uint8_t M1);
void reconnect(const char *topic);
void check(const char * topic);
