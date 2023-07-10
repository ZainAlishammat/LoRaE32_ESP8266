#ifndef ARDUINO
    #include <Arduino.h>
#endif

#ifndef EBYTE_LORA_E32_LIBRARY_H
    #include "LoRa_Config.h"
#endif

#include "recievedData.h"



void loraPinModeSetup(void);
void loraConfigSet(void);
boolean lora_Set_Mode(void);
boolean LoRa_sendMsg(const char *msg, const uint8_t size);
boolean LoRa_recieveMsg(LoRaReceivedDataStruct_t rd);
void printParameters(struct Configuration configuration);






