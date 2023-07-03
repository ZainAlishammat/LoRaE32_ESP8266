#include "LoRaE32_ESP8266_MQTT.h"

// Instances
LoRa_E32_Config_Parameter loraConfigPar;
LoRa_E32_Config_Pin loraConfigPin;
SoftwareSerial lora_TxRx(loraConfigPin.LORA_Rx, loraConfigPin.LORA_Tx); /* UART Swaping*/
LoRa_E32 LORA_E32(&lora_TxRx, loraConfigPin.GPIO_AUX_LORA, loraConfigPin.GPIO_M0_LORA, loraConfigPin.GPIO_M1_LORA);

/*set the pinMode M0, M2 as output and AUX as input*/
void loraPinModeSetup(void)
{
    pinMode(loraConfigPin.GPIO_M0_LORA, OUTPUT);
    pinMode(loraConfigPin.GPIO_M1_LORA, OUTPUT);
    pinMode(loraConfigPin.GPIO_AUX_LORA, INPUT);
}

void loraConfigSet(void)
{
    loraPinModeSetup();
    LORA_E32.begin(); // Startup all pins and UART

    MODE_TYPE mode = LORA_E32.getMode();
    if (mode == MODE_3_SLEEP)
    {
        ResponseStructContainer response = LORA_E32.getConfiguration();
        Configuration moduleConfig = *(Configuration *)response.data;                          // Get configuration pointer before all other operation
        moduleConfig.ADDL = loraConfigPar.LORA_E32_ADDL;                                       // Modul low address
        moduleConfig.ADDH = loraConfigPar.LORA_E32_ADDH;                                       // Modul high address
        moduleConfig.CHAN = loraConfigPar.LORA_E32_CHAN;                                       // Modul channel
        moduleConfig.OPTION.transmissionPower = loraConfigPar.TRANSISSION_POWER;               // transmission power at 27 dBm
        moduleConfig.OPTION.ioDriveMode = loraConfigPar.IO_DRIVE_MODE;                         // UART pins as push pull
        moduleConfig.OPTION.wirelessWakeupTime = loraConfigPar.WIRELESS_WAKE_UP_TIME;          // wakeup time at 500 ms
        moduleConfig.SPED.uartParity = loraConfigPar.UART_PARITY;                              // UART 8N1
        moduleConfig.SPED.uartBaudRate = loraConfigPar.UAR_BAUD_DATA_RATE;                     // UART baudrate at 9600
        moduleConfig.SPED.airDataRate = loraConfigPar.AIR_DATA_RATE;                           // ait data rate at 19,2k pps
        moduleConfig.OPTION.fixedTransmission = loraConfigPar.FIXED_TRANSMISSION_ENABLING_BIT; // fixed transmission mode enable
        LORA_E32.setConfiguration(moduleConfig, loraConfigPar.PARAMETER_WITH_POWER_RESET);     // save the configuration after module reset
        Serial.println(response.status.getResponseDescription());
        Serial.println(response.status.code);
        printParameters(moduleConfig); // print module parameters after setting
        response.close();              // don't display the parameter anymore (Just one time)
    }
}

/* Configuring the lora_e32 mode over M0 and M2 pin_state

      MODE_0_NORMAL
      MODE_1_WAKE_UP
      MODE_2_POWER_SAVING
      MODE_3_SLEEP
*/

boolean lora_Set_Mode(MODE_TYPE mode)
{

    Status modeStatus;
    modeStatus = LORA_E32.setMode(mode);
    if (modeStatus == E32_SUCCESS)
    {
        printf("The mode %d%s\n", mode, " has been set seccessfully");
        return true;
    }
    else
    {
        printf("Isn't able to set the mode %d%s\n", mode, "seccessfully");
        return false;
    }

    return false;
}

boolean LoRa_sendMsg(const char *msg, const uint8_t size)
{
    MODE_TYPE mode = LORA_E32.getMode();

    if (mode != MODE_3_SLEEP || mode != MODE_2_POWER_SAVING)
    {
        ResponseStatus status;
        status = LORA_E32.sendFixedMessage(loraConfigPar.LORA_E32_ADDH, loraConfigPar.LORA_E32_ADDL, loraConfigPar.LORA_E32_CHAN, (void *)msg, size);
        if (status.code == E32_SUCCESS)
        {
            printf("Transmission succeed\n");
            return true;
        }

        else
        {
            printf("Transmission faild\n");
            return false;
        }
    }
    else
    {
        printf("The modul isn't in the correct mode!\n");
    }
    return false;
}

boolean LoRa_recieveMsg(receivedDataStruct rd)
{
    MODE_TYPE mode = LORA_E32.getMode();

    if (mode != MODE_3_SLEEP)
    {
        ResponseStructContainer dataComingContainer;
        dataComingContainer = LORA_E32.receiveMessage(SIZE_RECIEVED_DATA_LORA);
        if (dataComingContainer.status.code == E32_SUCCESS)
        {
            printf("Massage recieved\n");
            rd.data = &(dataComingContainer.data);
            memcpy(rd.rc_Data, (char *)dataComingContainer.data, sizeof(rd.rc_Data));
            dataComingContainer.close(); // free allocated memory space by malloc
            return true;
        }
    }
    else
    {
        printf("The modul isn't in the correct mode!\n");
        return false;
    }
    return NULL;
}

void printParameters(struct Configuration configuration)
{
    Serial.println("----------------------------------------");

    Serial.print(F("HEAD BIN: "));
    Serial.print(configuration.HEAD, BIN);
    Serial.print(" ");
    Serial.print(configuration.HEAD, DEC);
    Serial.print(" ");
    Serial.println(configuration.HEAD, HEX);
    Serial.println(F(" "));
    Serial.print(F("AddH BIN: "));
    Serial.println(configuration.ADDH, BIN);
    Serial.print(F("AddL BIN: "));
    Serial.println(configuration.ADDL, BIN);
    Serial.print(F("Chan BIN: "));
    Serial.print(configuration.CHAN, DEC);
    Serial.print(" -> ");
    Serial.println(configuration.getChannelDescription());
    Serial.println(F(" "));
    Serial.print(F("SpeedParityBit BIN    : "));
    Serial.print(configuration.SPED.uartParity, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDataRate BIN : "));
    Serial.print(configuration.SPED.uartBaudRate, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getUARTBaudRate());
    Serial.print(F("SpeedAirDataRate BIN  : "));
    Serial.print(configuration.SPED.airDataRate, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getAirDataRate());

    Serial.print(F("OptionTrans BIN       : "));
    Serial.print(configuration.OPTION.fixedTransmission, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getFixedTransmissionDescription());
    Serial.print(F("OptionPullup BIN      : "));
    Serial.print(configuration.OPTION.ioDriveMode, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getIODroveModeDescription());
    Serial.print(F("OptionWakeup BIN      : "));
    Serial.print(configuration.OPTION.wirelessWakeupTime, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
    Serial.print(F("OptionFEC BIN         : "));
    Serial.print(configuration.OPTION.fec, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getFECDescription());
    Serial.print(F("OptionPower BIN       : "));
    Serial.print(configuration.OPTION.transmissionPower, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getTransmissionPowerDescription());

    Serial.println("----------------------------------------");
}