#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <stdio.h>

#define MQTT_PortServer 1883

const char *Client_ID = "Zeus";
const char *MQTT_ = "192.168.1.178";

uint8_t MAC_Addresse[6];
boolean c_ID;

void callback_Set(void);

// Instances
WiFiClient ESP_CLIENT;
PubSubClient P_S_CLIENT(ESP_CLIENT);

void setupWiFi(const char *SSID, const char *password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, password);
    printf("Attempt to connect to %s", SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    printf("Connection with %s%s\r\n", SSID, " is done!");
    Serial.println(WiFi.localIP());
}

void stayAwake()
{
    P_S_CLIENT.loop();
}

void MQTTMakeConnection()
{
    IPAddress MQTT_IPServer(192, 168, 1, 178);
    P_S_CLIENT.setServer(MQTT_IPServer, MQTT_PortServer);
    c_ID = P_S_CLIENT.connect(Client_ID);
    callback_Set();
}

boolean publishMsg(const char *topic, const char *payload)
{
    boolean published = false;
    if (P_S_CLIENT.connected())
    {
        published = P_S_CLIENT.publish(topic, payload);
        return published;
    }
    return published;
}

boolean publishNum(const char *topic, const uint8_t *payload, unsigned int length)
{
    boolean published = false;
    if (P_S_CLIENT.connected())
    {
        published = P_S_CLIENT.publish(topic, payload, length);
        return published;
    }
    return published;
}

boolean subscribeTopic(const char *topic)
{
    boolean subscried = false;

    printf("Trying to connect with %s\n", MQTT_);

    if (P_S_CLIENT.connected())
    {
        printf("connected\n");
        subscried = P_S_CLIENT.subscribe(topic);
        printf("Subscribing the topic %s%s\n", topic, " was successfully");
        return subscried;
    }
    return subscried;
}

void __subCallback(char *topic, byte *payload, unsigned int length)
{
    printf("Message arrieved from %s\n", topic);
    uint8_t i;
    for (i = 0; i < length; i++)
    {
        printf("%c", (char)payload[i]);
    }
    printf("\r\n");
}
void callback_Set(void)
{
    P_S_CLIENT.setCallback(__subCallback);
}
boolean unsubscribeTopic(const char *topic)
{

    boolean unsubscribed = false;
    if (P_S_CLIENT.connected())
    {
        unsubscribed = P_S_CLIENT.unsubscribe(topic);
        printf("Unsubscribing the topic %s%s\n", topic, " was successfully");
        return unsubscribed;
    }
    return unsubscribed;
}

void reconnect(const char *topic)
{
    printf("Attempting MQTT connection...\n");

    while (!P_S_CLIENT.connected())
    {
        if (P_S_CLIENT.connect(Client_ID))
        {
            subscribeTopic(topic);
        }
        else
        {
            Serial.print("failed to reconnet. ");
            Serial.println(P_S_CLIENT.state());
            printf("try again in 5 seconds/n");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void check(const char *topic)
{
    if (!P_S_CLIENT.connected())
    {
        reconnect(topic);
    }
}