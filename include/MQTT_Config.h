
#ifndef MQTT_Config_H__
#define MQTT_Config_H__ 

#define MQTT_MAX_BUFFER_SIZE 512
#define MQTT_PortServer 1883 // MQTT port number
#define QoS 0                // MQTT quality of service


 struct MQTT_CONFIG_STRUCT{

    MQTT_CONFIG_STRUCT(){

                    WIFI_SSID = "OpenWrt";
                    WIFI_PASSWORD = "12345678";
                    wifi_Data[0] = WIFI_SSID;
                    wifi_Data[1] = WIFI_PASSWORD;
                    wifiSetMode = WIFI_STA;
                    MQTT_IPAddresse[0] = 192;
                    MQTT_IPAddresse[1] = 168;
                    MQTT_IPAddresse[2] = 1;
                    MQTT_IPAddresse[3] = 178;
                    MQTT_CLIENT_ID = "Zeus";
    }
    const char *WIFI_SSID;                       // wifi name
    const char *WIFI_PASSWORD ;                  // wifi password
    const char *wifi_Data[2];
    WiFiMode wifiSetMode = WIFI_STA;             // WIFI_OFF, WIFI_STA, WIFI_AP, WIFI_AP_STA
    uint8_t MQTT_IPAddresse[4];                  // The mqtt broker's ip addresse
    const char *MQTT_CLIENT_ID;                  // Mqtt client id 
    uint8_t MAC_Addresse[6];

};    


#endif