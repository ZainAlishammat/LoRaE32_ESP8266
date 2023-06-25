#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <stdio.h>

#define MQTT_PortServer 1883 // MQTT port number
#define QoS 0                // MQTT quality of service

void setupWiFi(const char *SSID, const char *password, WiFiMode wifimode);
void MQTTMakeConnection();
boolean MQTT_stayAwake(void);
boolean publishMsg(const char *topic, const char *payload);
boolean publishNum(const char *topic, const uint8_t *payload, unsigned int length);
boolean subscribeTopic(const char *topics[], uint8_t size);
boolean unsubscribeTopic(const char *topic);
void __subCallback(char *topic, byte *payload, unsigned int length);
// void __handleInterrupt_UART_Rx();
// void lora_Inspector(void);
// void lora_Set_Mode(uint8_t M0, uint8_t M1);
void MQTT_check(const char *topics[], uint8_t size);
