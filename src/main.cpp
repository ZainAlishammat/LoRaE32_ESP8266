#include <Arduino.h>
#ifndef WiFiClient
#include <ESP8266WiFi.h>
#endif
#include <stdio.h>
#include <interrupts.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <LoRa_E32.h>
#include "func.h"

// #include <AsyncMqttClient.h>

// Lora Modul Pins SET
#define GPIO_M0_LORA D1
#define GPIO_M1_LORA D2
#define INTERRUPT_PIN D5
#define LORA_Tx D8
#define LORA_Rx D7

#define QoS 0

const char *topic = "dummyPub";

// Funktion deklarations
void setupWiFi(const char *SSID, const char *password);
void __handleInterrupt_UART_Rx();
void lora_Inspector(void);
void lora_Set_Mode(uint8_t M0, uint8_t M1);
void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);

// uint8_t MAC_Addresse[6];
volatile uint8_t saveRxData[100];
char reciveMsg;
boolean subTopic;

// Instances
SoftwareSerial lora_TxRx(LORA_Rx, LORA_Tx); /* UART Swaping*/
LoRa_E32 LORA_E32(&lora_TxRx);

void setup()
{
  pinMode(GPIO_M0_LORA, HIGH);
  pinMode(GPIO_M1_LORA, HIGH);
  pinMode(INTERRUPT_PIN, HIGH);

  // pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), __handleInterrupt_UART_Rx, FALLING);
  Serial.begin(1200);
  delay(500);
  setupWiFi("OpenWrt", "12345678");
  MQTTMakeConnection();
  callback_Set();
  delay(100);
  subTopic = subscribeTopic(topic);
  delay(200);
  LORA_E32.begin(); // Startup all pins and UART
  ResponseStructContainer res;
  res = LORA_E32.getConfiguration();
  Configuration configuration = *(Configuration *)res.data;
  Serial.println(res.status.getResponseDescription());
  Serial.println(res.status.code);
  printParameters(configuration);
  ResponseStructContainer cMi;
  cMi = LORA_E32.getModuleInformation();
  ModuleInformation mi = *(ModuleInformation *)cMi.data;

  Serial.println(cMi.status.getResponseDescription());
  Serial.println(cMi.status.code);

  printModuleInformation(mi);

  res.close();
  cMi.close();
}
void loop()
{
  stayAwake(); // Maintaining regularly connection to the server for incoming messages
  boolean ch = check(topic);
  printf("%d\n", ch);

  delay(5000);
}

void setupWiFi(const char *SSID, const char *password)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, password);
  printf("Attempt to connect to %s", SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  printf("Connection with %s%s\r\n", SSID, " is done!");
  Serial.println(WiFi.localIP());
}

void __handleInterrupt_UART_Rx()
{
  while (lora_TxRx.available() > 0)
  {
    printf("Data recived!\n");

    int msg = lora_TxRx.read();
    strcpy((char *)saveRxData, (char *)msg);
    // Clear internal buff for new incomming bytes
    lora_TxRx.flush();
  }
}

void UART_Tx(uint16_t msg)
{
  /*
  AUX = 1, user can input data less than 512 bytes continously withou overflow
  AUX = 0, Buffer not empty -> the internel 512 bytes in ther buffer have not been wirten to the RFIC completely

  */

  if (digitalRead(INTERRUPT_PIN) == HIGH)
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
void printModuleInformation(struct ModuleInformation moduleInformation)
{
  Serial.println("----------------------------------------");
  Serial.print(F("HEAD BIN: "));
  Serial.print(moduleInformation.HEAD, BIN);
  Serial.print(" ");
  Serial.print(moduleInformation.HEAD, DEC);
  Serial.print(" ");
  Serial.println(moduleInformation.HEAD, HEX);

  Serial.print(F("Freq.: "));
  Serial.println(moduleInformation.frequency, HEX);
  Serial.print(F("Version  : "));
  Serial.println(moduleInformation.version, HEX);
  Serial.print(F("Features : "));
  Serial.println(moduleInformation.features, HEX);
  Serial.println("----------------------------------------");
}
