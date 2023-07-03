 #include <LoRa_E32.h>

#ifndef LoRa_Config_H__
#define LoRa_Config_H__


enum PARAMETER_MODE_POWER_RESET
{
  SAVE_PWR_DOWN         = 0xC0,
  NO_SAVE_PWR_DOWN      = 0xC2
};

 struct LoRa_E32_Config_Parameter{

  LoRa_E32_Config_Parameter(){

                    LORA_E32_HEAD = 0x0;
                    LORA_E32_ADDH = 0x5;
                    LORA_E32_ADDL = 0x1;
                    LORA_E32_CHAN = 0x1;
                    UART_PARITY = 0b00;
                    UAR_BAUD_DATA_RATE= 0b111;
                    AIR_DATA_RATE= 0b101;
                    FIXED_TRANSMISSION_ENABLING_BIT= 0b1;
                    IO_DRIVE_MODE= 0b1;
                    WIRELESS_WAKE_UP_TIME= 0b001;
                    FEC_SWITCH= 0b1;
                    TRANSISSION_POWER= 0b01;
                    PARAMETER_WITH_POWER_RESET = WRITE_CFG_PWR_DWN_SAVE; // SAVE THE PARAMETERS AFTER RESET
  }

    byte LORA_E32_HEAD;
    byte LORA_E32_ADDH;
    byte LORA_E32_ADDL;
    byte LORA_E32_CHAN;
    byte UART_PARITY;
    byte UAR_BAUD_DATA_RATE;
    byte AIR_DATA_RATE;
    byte FIXED_TRANSMISSION_ENABLING_BIT;
    byte IO_DRIVE_MODE;
    byte WIRELESS_WAKE_UP_TIME;
    byte FEC_SWITCH;
    byte TRANSISSION_POWER;
    PROGRAM_COMMAND PARAMETER_WITH_POWER_RESET; // SAVE THE PARAMETERS AFTER RESET

};


 struct LoRa_E32_Config_Pin{

      LoRa_E32_Config_Pin()
    {
        LORA_Tx = D8;
        LORA_Rx = D7;
        GPIO_M0_LORA = D1;
        GPIO_M1_LORA = D2;
        GPIO_AUX_LORA = D5;
    }
    byte LORA_Tx;       // ESP8266 transmitting pin
    byte LORA_Rx;       // ESP8266 receiving pin
    byte GPIO_M0_LORA;  // ESP8266 m0 pin
    byte GPIO_M1_LORA;  // ESP8266 m1 pin
    byte GPIO_AUX_LORA; // ESP8266 aux pin

};

#endif