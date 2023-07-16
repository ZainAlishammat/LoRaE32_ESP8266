#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <stdio.h>
#include "MQTT_Config.h"




void setupWiFi(const char *SSID, const char *password, WiFiMode wifimode);
void MQTTMakeConnection(void);
boolean MQTT_stayAwake(void);
boolean publisheMsg(const char* topic, const char payload [], const uint8_t p_len, boolean is_retaind);
boolean subscribeTopic(const char *topics[], uint8_t size);
boolean unsubscribeTopic(const char *topic);
void __subCallback(char *topic, byte *payload, unsigned int length);
void MQTT_check(const char *topics[], uint8_t size);
