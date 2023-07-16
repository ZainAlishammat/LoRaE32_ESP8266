#ifndef ARDUINO
    #include <Arduino.h>
#endif

#ifndef EBYTE_LORA_E32_LIBRARY_H
    #include "LoRa_Config.h"
#endif





void loraPinModeSetup(void);
void loraConfigSet(MODE_TYPE mode);
Status lora_Set_Mode(MODE_TYPE mode);
boolean loraSendString(const String msg);
boolean loraReceiveMsg(char rd[], const uint8_t size);
String loraReceiveString();
void loraPinState();
void printParameters(struct Configuration configuration);






