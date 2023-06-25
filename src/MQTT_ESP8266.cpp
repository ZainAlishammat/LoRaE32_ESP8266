#include "MQTT_ESP8266.h"
#include "MQTT_Config.h"

MQTT_CONFIG_STRUCT mqtt_cs;          // make a configuration instanse for setting wifi and mqtt connection requirements
WiFiClient ESP_CLIENT;               // set up a WiFiClient instance for wifi conncetion with the broker
PubSubClient P_S_CLIENT(ESP_CLIENT); // make a PubSubClient instance for mqtt setting

boolean c_ID;

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
void setupWiFi(const char *SSID, const char *password, WiFiMode wifimode)
{

    boolean mode = WiFi.mode(wifimode);
    if (mode)
    {
        WiFi.begin(SSID, password); // ssid and password of the wifi netzwerk
        printf("Attempt to connect to %s", SSID);
        while (WiFi.status() != WL_CONNECTED) // attempting wifi connection
        {
            delay(500);
            printf(".");
        }
        printf("Connection with %s%s\r\n", SSID, " is done!");
        Serial.println(WiFi.localIP());
    }
    else
    {
        printf("Setting mode is failed!\n");
    }
}

/*maintaining a connection with the mqtt broker*/
boolean MQTT_stayAwake(void)
{
    boolean loopCheck = P_S_CLIENT.loop();
    return loopCheck;
}

/*build a connection with the mqtt broker*/
void MQTTMakeConnection()
{
    setupWiFi(mqtt_cs.WIFI_SSID, mqtt_cs.WIFI_PASSWORD, mqtt_cs.wifiSetMode);
    P_S_CLIENT.setServer(mqtt_cs.MQTT_IPAddresse, MQTT_PortServer); // set server connection with mqtt broker
    c_ID = P_S_CLIENT.connect(mqtt_cs.MQTT_CLIENT_ID);              // creat a client id in the mqtt broker
    P_S_CLIENT.setCallback(__subCallback);
}

/*functoin for publishing massages
user has to enter  the topic and payload
Ther state of the publishing is gonna be returned
*/
boolean publishMsg(const char *topic, const char *payload)
{
    boolean connected = P_S_CLIENT.connected();
    boolean published = false;
    if (connected) // check if there is a connection the the mqtt broker
    {
        published = P_S_CLIENT.publish(topic, payload);
        delay(1000);
        return published;
    }
    else
    {
        printf("There is no connection with the MQTT Broker!\n");
    }
    return published;
}

/*functoin for publishing numbers
user has to enter  the topic and payload and the length of the payload
Ther state of the publishing is gonna be returned
*/
boolean publishNum(const char *topic, const uint8_t *payload, unsigned int length)
{
    boolean connected = P_S_CLIENT.connected();
    boolean published = false;
    if (connected) // check if there is a connection the the mqtt broker
    {
        published = P_S_CLIENT.publish(topic, payload, length);
        delay(1000);
        return published;
    }
    else
    {
        printf("There is no connection with the MQTT Broker!\n");
    }
    return published;
}

/*functoin for subscribing purposes
The user has to put the topic to be subscribed
The state of the subscribing is gonna be returned*/
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

/* function for unsubscribing purposes
The user has to input the topic to be unsubscribed*/
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

/*keeping a regular connection with the broker und regular subscription*/
void MQTT_check(const char *topics[], uint8_t size)
{
    boolean connected = P_S_CLIENT.connected();
    if (!connected)
    {
        printf("Attempt to make a MQTT connection...\n");
        while (!connected) // stay hanging wihle there is no connection wiht the mqtt broker
        {
            c_ID = P_S_CLIENT.connect(mqtt_cs.MQTT_CLIENT_ID); // if there is a connection, check if the client id is accepted
            if (c_ID)
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