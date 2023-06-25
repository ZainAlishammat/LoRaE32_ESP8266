 #include <LoRa_E32.h>


enum PARAMETER_MODE_POWER_RESET
{
  SAVE_PWR_DOWN         = 0xC0,
  NO_SAVE_PWR_DOWN      = 0xC2
};



typedef struct {
    byte LORA_E32_HEAD = 0x0;
    byte LORA_E32_ADDH = 0x5;
    byte LORA_E32_ADDL = 0x1;
    byte LORA_E32_CHAN = 0x1;
    byte UART_PARITY = 0b00;
    byte UAR_BAUD_DATA_RATE= 0b011;
    byte AIR_DATA_RATE= 0b101;
    byte FIXED_TRANSMISSION_ENABLING_BIT= 0b1;
    byte IO_DRIVE_MODE= 0b1;
    byte WIRELESS_WAKE_UP_TIME= 0b001;
    byte FEC_SWITCH= 0b1;
    byte TRANSISSION_POWER= 0b01;
    PROGRAM_COMMAND PARAMETER_WITH_POWER_RESET = (PROGRAM_COMMAND)SAVE_PWR_DOWN;

}LoRa_E32_Config_Parameter;


typedef struct {
    byte LORA_Tx = D8;       // ESP8266 transmittng pin
    byte LORA_Rx = D7;       // ESP8266 reciving ping
    byte GPIO_M0_LORA = D1;  // ESP8266 m0 pin
    byte GPIO_M1_LORA = D2;  // ESP8266 m1 pin
    byte GPIO_AUX_LORA = D5; // ESP8266 aux ping

}LoRa_E32_Config_Pin;