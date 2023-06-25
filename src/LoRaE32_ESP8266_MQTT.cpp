#include "LoRaE32_ESP8266_MQTT.h"

#define LORA_Tx D8       // set lora transmittng pin
#define LORA_Rx D7       // set lora reciving ping
byte GPIO_M0_LORA = D1;  // set lora m0 pin
byte GPIO_M1_LORA = D2;  // set lora m1 pin
byte GPIO_AUX_LORA = D5; // set lora aux ping

void printParameters(struct Configuration configuration);

// Instances
SoftwareSerial lora_TxRx(LORA_Rx, LORA_Tx); /* UART Swaping*/
LoRa_E32 LORA_E32(&lora_TxRx, GPIO_AUX_LORA, GPIO_M0_LORA, GPIO_M1_LORA);

/*set the pinMode M0, M2 as output and AUX as input*/
void loraPinModeSetup(void)
{
    pinMode((uint8_t)GPIO_M0_LORA, OUTPUT);
    pinMode((uint8_t)GPIO_M1_LORA, OUTPUT);
    pinMode((uint8_t)GPIO_AUX_LORA, INPUT);
}

void loraConfigSet(void)
{
    loraPinModeSetup();
    lora_TxRx.begin(9600); // set up a serial connection with LoRa E32 over UART
    LORA_E32.begin();      // Startup all pins and UART
    ResponseStructContainer response = LORA_E32.getConfiguration();
    Configuration moduleConfig = *(Configuration *)response.data;    // Get configuration pointer before all other operation
    moduleConfig.OPTION.transmissionPower = POWER_27;                // transmission power at 27 dBm
    moduleConfig.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS; // UART pins as push pull
    moduleConfig.OPTION.wirelessWakeupTime = WAKE_UP_500;            // wakeup time at 500 ms
    moduleConfig.SPED.uartParity = MODE_00_8N1;                      // UART 8N1
    moduleConfig.SPED.uartBaudRate = UART_BPS_9600;                  // UART baudrate at 9600
    moduleConfig.SPED.airDataRate = AIR_DATA_RATE_101_192;           // ait data rate at 19,2k pps
    moduleConfig.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;   // fixed transmission mode enable
    LORA_E32.setConfiguration(moduleConfig, WRITE_CFG_PWR_DWN_SAVE); // save the configuration after module reset
    Serial.println(response.status.getResponseDescription());
    Serial.println(response.status.code);
    printParameters(moduleConfig); // print module parameters after setting
    response.close();              // don't display the parameter anymore (Just one time)
}

// void __handleInterrupt_Rx_LoRa()
// {
//     while (lora_TxRx.available() > 0)
//     {
//         printf("Data is received!\n");

//         int msg = lora_TxRx.read();
//         Serial.println(msg);
//         strcpy((char *)saveRxData, (char *)msg);
//     }

//     // Clear internal buff for new incomming bytes
//     lora_TxRx.flush();
// }

/* Listinging to AUX pin wenn go low for reading data
 attachInterrupt(digitalPinToInterrupt(GPIO_AUX_LORA), __handleInterrupt_Rx_LoRa, FALLING);*/

void Tx_LoRa(uint16_t msg)
{
    /*
    AUX = 1, user can input data less than 512 bytes continously withou overflow
    AUX = 0, Buffer not empty -> the internel 512 bytes in ther buffer have not been wirten to the RFIC completely

    */

    if (digitalRead(GPIO_AUX_LORA) == HIGH)
    {

        lora_TxRx.write(msg);
    }
    else
    {
        lora_TxRx.println("Sending data faild -> Buffer not empty!");
    }
}

/*configuring the lora_e32 mode over M0 and M2 pin_state
    Mode 0: M0 & M1 = LOW (Normal mode)
    Mode 1: M0 = HIGH & M1 = LOW (Wake up mode)
    Mode 2: M0 = LOW & M1 = HIGH (Power saving mode)
    Mode 3: M0 = HIGH & M1 = LOW (Sleep mode)


*/

void lora_Set_Mode(uint8_t M0, uint8_t M1)
{
    // LoRa_E32.setMode();
    digitalWrite((uint8_t)GPIO_M0_LORA, M0);
    digitalWrite((uint8_t)GPIO_M1_LORA, M1);
}

void lora_Inspector(void)
{
    // set pin M0 and M1 high for configuring the lora module
    lora_Set_Mode(HIGH, HIGH);

    if ((digitalRead(GPIO_M0_LORA) & digitalRead(GPIO_M1_LORA)) == HIGH)
    {
        // for default, baud rate is 9600
        lora_TxRx.begin(9600);
    }
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