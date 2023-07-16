#include "LoRaE32_ESP8266_MQTT.h"

// Instances from LoRa_Config.h for setting the module paramters and pins
LoRa_E32_Config_Parameter loraConfigPar;
LoRa_E32_Config_Pin loraConfigPin;
// Instance from SoftwareSerial for opining a UART connection with the lora module
SoftwareSerial lora_TxRx(loraConfigPin.LORA_Rx, loraConfigPin.LORA_Tx); /* UART Swaping*/
// Instances of the LoRa_E32 library
LoRa_E32 LORA_E32(loraConfigPin.LORA_Tx, loraConfigPin.LORA_Rx, loraConfigPin.GPIO_AUX_LORA, loraConfigPin.GPIO_M0_LORA, loraConfigPin.GPIO_M1_LORA);

/*
This fucntion must be called at first once for setting the serial wire connection with
the lora module and setting the required mode
According to the mode set in the LoRa_Config.h and the parameters for the module
this function will set these parameters
Only if the module in the sleep mode this function will set the parameters for the module

*/
void loraConfigSet(MODE_TYPE mode)
{
    lora_Set_Mode(mode);
    LORA_E32.begin(); // Startup all pins and UART
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
        moduleConfig.OPTION.fec = loraConfigPar.FEC_SWITCH;
        LORA_E32.setConfiguration(moduleConfig, loraConfigPar.PARAMETER_WITH_POWER_RESET); // save the configuration after module reset
        Serial.println(response.status.getResponseDescription());
        Serial.println(response.status.code);
        printParameters(moduleConfig); // print module parameters after setting
        response.close();              // don't display the parameter anymore (Just one time)
    }
}

/*set the pinMode M0, M2 as output and AUX as input*/
void loraPinModeSetup(void)
{
    pinMode(loraConfigPin.GPIO_M0_LORA, OUTPUT);
    pinMode(loraConfigPin.GPIO_M1_LORA, OUTPUT);
    pinMode(loraConfigPin.GPIO_AUX_LORA, INPUT);
}

/*
Check the state of the AUS, M0 and M1 in order to make sure that
the sending or receiving data beeing handeld correct
*/
void loraPinState()
{
    Serial.print("AUX State is -> ");
    Serial.println(digitalRead(loraConfigPin.GPIO_AUX_LORA));
    Serial.print("M0 State is -> ");
    Serial.println(digitalRead(loraConfigPin.GPIO_M0_LORA));
    Serial.print("M1 State is -> ");
    Serial.println(digitalRead(loraConfigPin.GPIO_M1_LORA));
}

/*
Calling this function will set the mode in the LoRa.Config.h over M0 and M2 pin_state

      MODE_0_NORMAL
      MODE_1_WAKE_UP
      MODE_2_POWER_SAVING
      MODE_3_SLEEP

*@return true if the desired mode has been set successfully
*/
Status lora_Set_Mode(MODE_TYPE mode)
{
    Status modeStatus = LORA_E32.setMode(mode);
    if (modeStatus == E32_SUCCESS)
    {
        printf("The mode %d%s\n", mode, " has been set seccessfully");
        return modeStatus;
    }
    else
    {
        printf("Not able to set the mode %d%s\n ", mode, "seccessfully");
        return modeStatus;
    }

    return modeStatus;
}

/*
Sending a string message
*@param msg: the message to be sent
*@return true of the message has been sent seccessfully out of the ESP module
*@return false if the message hasn't been sent successfully out of the module or the module isn't in the correct mode
*/

boolean loraSendString(const String msg)
{

    if (LORA_E32.getMode() != MODE_3_SLEEP && LORA_E32.getMode() != MODE_2_POWER_SAVING)
    {
        ResponseStatus status;
        status = LORA_E32.sendFixedMessage(loraConfigPar.LORA_E32_ADDH, loraConfigPar.LORA_E32_ADDL, loraConfigPar.LORA_E32_CHAN, msg);

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

/*
Receive a message form the lora modul
*@param rd; a char array that store the data comming
*@param size: the number of data in bytes to be stored
*@return true of the requird data has been all received and stored in the buffer
*@return false if the module isn't in the correct mode or the received data has been stored
*/

boolean loraReceiveMsg(char rd[], const uint8_t size)
{
    if (LORA_E32.getMode() != MODE_3_SLEEP && size > 0)
    {
        while (LORA_E32.available())
        {
            ResponseStructContainer dataComingContainer;
            // dataComingContainer.data = (uint8_t *)dataComingContainer.data;
            dataComingContainer = LORA_E32.receiveMessage(size);
            if (dataComingContainer.status.code == E32_SUCCESS)
            {
                printf("Massage recieved\n");
                memcpy(rd, (char *)LORA_E32.receiveMessage(size).data, size);

                return true;
            }
            else
            {

                Serial.println("Recieving data encourted with errors");
                return false;
            }
        }
    }
    else
    {
        Serial.println("The modul isn't in the correct mode!");
        return false;
    }
    return false;
}

/*
Receive a message string form the lora modul
*@return a string of the received data

*/
String loraReceiveString()
{
    ResponseContainer rc;

    if (LORA_E32.getMode() != MODE_3_SLEEP)
    {
        rc = LORA_E32.receiveMessage();
        return rc.data;
    }

    else
    {
        Serial.println("The modul isn't in the correct mode");
    }
    return rc.data;
}

/*
Print the module configuration and parameters
It's recommended to use only on the 3 mode (sleep mode)
*@param configuration: a struct of the configuration to be set
*/
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