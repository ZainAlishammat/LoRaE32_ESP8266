
#include <Arduino.h>
#include <LoRa_E32.h>
#include <SoftwareSerial.h>

#define GPIO_M0_LORA D1  // set lora m0 pin
#define GPIO_M1_LORA D2  // set lora m1 pin
#define GPIO_AUX_LORA D5 // set lora aux ping
#define LORA_Tx D8       // set lora transmittng pin
#define LORA_Rx D7       // set lora reciving ping

void printParameters(struct Configuration configuration);

// Instances
SoftwareSerial lora_TxRx(LORA_Rx, LORA_Tx); /* UART Swaping*/
LoRa_E32 LORA_E32(&lora_TxRx);

void loraPinModeSetup(void)
{
    // set always the pins M1 and M0 as output and AUX as Input
    pinMode(GPIO_M0_LORA, OUTPUT);
    pinMode(GPIO_M1_LORA, OUTPUT);
    pinMode(GPIO_AUX_LORA, INPUT);
}

void loraConfigSet(void)
{
    lora_TxRx.begin(9600);
    LORA_E32.begin(); // Startup all pins and UART
    ResponseStructContainer response = LORA_E32.getConfiguration();
    Configuration moduleConfig = *(Configuration *)response.data; // Get configuration pointer before all other operation
    // moduleConfig.ADDL = 0x01;
    // moduleConfig.ADDH = 0x00;
    // moduleConfig.CHAN = 0x00;
    moduleConfig.OPTION.transmissionPower = POWER_27;
    moduleConfig.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    moduleConfig.OPTION.wirelessWakeupTime = WAKE_UP_500;
    moduleConfig.SPED.uartParity = MODE_00_8N1;
    moduleConfig.SPED.uartBaudRate = UART_BPS_9600;
    moduleConfig.SPED.airDataRate = AIR_DATA_RATE_101_192;
    moduleConfig.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
    LORA_E32.setConfiguration(moduleConfig, WRITE_CFG_PWR_DWN_SAVE);
    Serial.println(response.status.getResponseDescription());
    Serial.println(response.status.code);
    printParameters(moduleConfig);
    response.close();
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
// Listinging to AUX pin wenn go low for reading data
// attachInterrupt(digitalPinToInterrupt(GPIO_AUX_LORA), __handleInterrupt_Rx_LoRa, FALLING);

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

void lora_Set_Mode(uint8_t M0, uint8_t M1)
{
    digitalWrite(GPIO_M0_LORA, M0);
    digitalWrite(GPIO_M1_LORA, M1);
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