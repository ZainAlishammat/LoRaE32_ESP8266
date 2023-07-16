#include "MQTT_ESP8266.h"

MQTT_CONFIG_STRUCT mqtt_cs;          // make a configuration instanse for setting wifi and mqtt connection requirements
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

/*
Set WiFi connection with the broker
*@param SSID: The wifi network name
*@param password: The wifi passowrd
*@param wifimodd: The desired wifi mode. It can be of the following: WIFI_OFF, WIFI_STA, WIFI_AP, WIFI_AP_STA
*/
void setupWiFi(const char *SSID, const char *password, WiFiMode wifimode)
{
    printf("Attempt to connect to %s\n ", SSID);

    boolean mode = WiFi.mode(wifimode);
    if (mode)
    {
        WiFi.begin(SSID, password);           // ssid and password of the wifi netzwerk
        while (WiFi.status() != WL_CONNECTED) // attempting wifi connection
        {
            delay(500);
            printf(".");
        }
        printf("Connection with %s%s\n", SSID, " is done!");
        Serial.println(WiFi.localIP());
    }
    else
    {
        printf("Setting mode is failed!\n");
    }
}

/*
Maintaining a connection with the mqtt broker
*/
boolean MQTT_stayAwake(void)
{
    boolean loopCheck = P_S_CLIENT.loop();
    return loopCheck;
}

/*
Build a connection with the mqtt broker
*/
void MQTTMakeConnection(void)
{
    setupWiFi(mqtt_cs.WIFI_SSID, mqtt_cs.WIFI_PASSWORD, mqtt_cs.wifiSetMode);
    P_S_CLIENT.setServer(mqtt_cs.MQTT_IPAddresse, MQTT_PortServer); // set server connection with mqtt broker
    P_S_CLIENT.connect(mqtt_cs.MQTT_CLIENT_ID);                     // creat a client id in the mqtt broker
    P_S_CLIENT.setCallback(__subCallback);
    P_S_CLIENT.setBufferSize(MQTT_MAX_PACKET_SIZE); // Set the max size of the buffer for the transceiver
}

/*
Publish a massage
*@param topic
*@param payload
*/
boolean publisheMsg(const char *topic, const char payload[], const uint8_t p_len, boolean is_retaind)
{
    uint16_t checkBufferSize = P_S_CLIENT.getBufferSize();
    boolean connected = P_S_CLIENT.connected();
    boolean published = false;
    if (checkBufferSize == 0)
        return Serial.println("Buffer size is invalid! :(");

    if (connected) // check if there is a connection the the mqtt broker
    {
        published = P_S_CLIENT.publish(topic, (uint8_t *)payload, p_len, is_retaind);
        delay(1000);
        return published;
    }
    else
    {
        printf("There is no connection with the MQTT Broker! :(\n");
    }
    return published;
}

/*
Subscribe a topic
*@param topics: an array of topcis
*@param size: the size of the array of topcis
*/
boolean subscribeTopic(const char *topics[], uint8_t size)
{
    boolean connected = P_S_CLIENT.connected();
    boolean subscried = false;

    if (connected) // check if there is a connection the the mqtt broker
    {
        printf("connected\n");
        for (uint8_t i = 0; i < size; i++)
        {
            subscried = P_S_CLIENT.subscribe(topics[i]);
            delay(2000);
        }
        return subscried;
    }
    else
    {
        printf("There is no connection with the MQTT Broker!\n");
    }
    return subscried;
}

/*
Unsubscribe to a topic
*@param topic
*/
boolean unsubscribeTopic(const char *topic)
{
    boolean connected = P_S_CLIENT.connected();
    boolean unsubscribed = false;
    if (connected) // check if there is a connection the the mqtt broker
    {
        unsubscribed = P_S_CLIENT.unsubscribe(topic);
        delay(1000);
        return unsubscribed;
    }
    else
    {
        printf("There is no connection with the MQTT Broker!\n");
    }
    return unsubscribed;
}

/*
Keeping a regular connection with the broker und regular subscription
*@param topics: An array of the topics to be subsscribed
*@param size: The size of the array of topics
*/
void MQTT_check(const char *topics[], uint8_t size)
{

    boolean connected = P_S_CLIENT.connected();
    if (!connected)
    {
        printf("Attempt to make a MQTT connection...\n");
        while (!connected) // stay hanging wihle there is no connection wiht the mqtt broker
        {
            // if there is a connection, check if the client id is accepted
            if (P_S_CLIENT.connect(mqtt_cs.MQTT_CLIENT_ID))
            {

                subscribeTopic(topics, size); // subscribe th the topic again
            }
            else
            {
                printf("failed to reconnet: Cleint ID isn't accepted!\n");
                printf("try again in 3 seconds\n");
                delay(3000);
            }
        }
    }
}