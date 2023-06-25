#include <Arduino.h>
#include "LoRa_Config.h"
#include "recievedData.h"



void loraPinModeSetup(void);
void loraConfigSet(void);
boolean lora_Set_Mode(MODE_TYPE mode);
boolean LoRa_sendMsg(const char *msg, const uint8_t size);
boolean LoRa_recieveMsg(receivedDataStruct rd);
void printParameters(struct Configuration configuration);






