


typedef struct {
    const char *WIFI_SSID = {"openWrt"};                       // wifi name
    const char *WIFI_PASSWORD = {"12345678"};                  // wifi password
    const char *wifi_Data[2] = {WIFI_SSID, WIFI_PASSWORD};
    WiFiMode wifiSetMode = WIFI_STA;                           // WIFI_OFF, WIFI_STA, WIFI_AP, WIFI_AP_STA
    const uint8_t MQTT_IPAddresse[4] = {192, 168, 1, 178};     // The mqtt broker's ip addresse
    const char *MQTT_CLIENT_ID = "Zeus";                       // Mqtt client id 
   uint8_t MAC_Addresse[6];

}MQTT_CONFIG_STRUCT;    
