#include "MQTT_ESP8266.h"

#define MQTT_PortServer 1883 // MQTT port number
#define QoS 0                // MQTT quality of service

const char *Client_ID = "Zeus";                 // enter a client id for MQTT connnection with the Broker
uint8_t MQTT_IPAddresse[] = {192, 168, 1, 178}; // enter the MQTT broker IP addresse
// IPAddress MQTT_IPServer(192, 168, 1, 178);     // enter the ip addresse of the mqtt broker

uint8_t MAC_Addresse[6]; // the mac addrese of the esp8266 module
boolean c_ID;

WiFiClient ESP_CLIENT;               // set up a WiFiClient instance for wifi conncetion with the broker
PubSubClient P_S_CLIENT(ESP_CLIENT); // make a PubSubClient instance for mqtt setting

/* MQTT callback function for subscription purposes*/
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

/*Set WiFi connection with the broker*/
void setupWiFi(const char *SSID, const char *password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, password); // ssid and password of the wifi netzwerk
    printf("Attempt to connect to %s", SSID);
    while (WiFi.status() != WL_CONNECTED) // check wifi connection
    {
        delay(500);
        Serial.print(".");
    }
    printf("Connection with %s%s\r\n", SSID, " is done!");
    Serial.println(WiFi.localIP());
}

/*maintaining a connection with the mqtt broker*/
void stayAwake()
{
    P_S_CLIENT.loop();
}

/*build a connection with the mqtt broker*/
void MQTTMakeConnection()
{
    P_S_CLIENT.setServer(MQTT_IPAddresse, MQTT_PortServer); // set server connection with mqtt broker
    c_ID = P_S_CLIENT.connect(Client_ID);                   // creat a client id in the mqtt broker
    P_S_CLIENT.setCallback(__subCallback);
}

/*functoin for publishing massages
user has to enter  the topic and payload
Ther state of the publishing is gonna be returned
*/
boolean publishMsg(const char *topic, const char *payload)
{
    boolean published = false;
    if (P_S_CLIENT.connected()) // check if there is a connection the the mqtt broker
    {
        published = P_S_CLIENT.publish(topic, payload);
        return published;
    }
    return published;
}

/*functoin for publishing numbers
user has to enter  the topic and payload and the length of the payload
Ther state of the publishing is gonna be returned
*/
boolean publishNum(const char *topic, const uint8_t *payload, unsigned int length)
{
    boolean published = false;
    if (P_S_CLIENT.connected()) // check if there is a connection the the mqtt broker
    {
        published = P_S_CLIENT.publish(topic, payload, length);
        return published;
    }
    return published;
}

/*functoin for subscribing purposes
The user has to put the topic to be subscribed
The state of the subscribing is gonna be returned*/
boolean subscribeTopic(const char *topic)
{
    boolean subscried = false;

    printf("Trying to connect with the MQTT Broker\n");

    if (P_S_CLIENT.connected()) // check if there is a connection the the mqtt broker
    {
        printf("connected\n");
        subscried = P_S_CLIENT.subscribe(topic);
        printf("Subscribing the topic %s%s\n", topic, " was successfully");
        return subscried;
    }
    return subscried;
}

/* function for unsubscribing purposes
The user has to input the topic to be unsubscribed*/
boolean unsubscribeTopic(const char *topic)
{

    boolean unsubscribed = false;
    if (P_S_CLIENT.connected()) // check if there is a connection the the mqtt broker
    {
        unsubscribed = P_S_CLIENT.unsubscribe(topic);
        printf("Unsubscribing the topic %s%s\n", topic, " was successfully");
        return unsubscribed;
    }
    return unsubscribed;
}

/*reconnect to the mqtt server if the connection is broken of failed*/
void reconnect(const char *topic)
{
    printf("Attempting MQTT connection...\n");

    while (!P_S_CLIENT.connected()) // stay hanging wihle there is no connection wiht the mqtt broker
    {
        if (P_S_CLIENT.connect(Client_ID)) // if there is a connection, check if the client id is accepted
        {
            subscribeTopic(topic); // subscribe th the topic again
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

/*keeping a regular connection with the broker und regular subscription*/
void check(const char *topic)
{
    if (!P_S_CLIENT.connected())
    {
        reconnect(topic);
    }
}