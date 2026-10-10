#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "esp_bt.h"
#include "esp_wifi.h"
#include <SPI.h>

SPIClass *vspi = nullptr;
SPIClass *hspi = nullptr;

// VSPI for RADIO 1
const uint8_t VSPI_CE_PIN = 4;
const uint8_t VSPI_CSN_PIN = 5;
const uint8_t VSPI_SCK = 18;
const uint8_t VSPI_MISO = 19;
const uint8_t VSPI_MOSI = 23;

/*
// HSPI for RADIO 2
const uint8_t HSPI_CE_PIN = 15;
const uint8_t HSPI_CSN_PIN = 16;
const uint8_t HSPI_SCK = 14;
const uint8_t HSPI_MISO = 12;
const uint8_t HSPI_MOSI = 13;
*/

//constants
const uint8_t LED_PIN = 2;
const uint8_t SPI_SPEED = 16000000;
const uint8_t MIN_CHANNEL = 2;
const uint8_t MAX_CHANNEL = 86;

unsigned int radio1_flag = 0;  // VSPI// Flag variable to keep track of direction
int radio1_ch = 45;            // Variable to store value of ch

/*
unsigned int radio2_flag = 0;  // HSPI// Flag variable to keep track of direction
int radio2_ch = 45;            // Variable to store value of ch
*/


// radio1 on VSPI
RF24 radio1(VSPI_CE_PIN, VSPI_CSN_PIN, SPI_SPEED);
//radio2 on HSPI
//RF24 radio2(HSPI_CE_PIN, HSPI_CSN_PIN, SPI_SPEED);

void goThroughChannelsMode()
{

    if (radio1_flag == 0)
    { // If flag is 0, increment ch by 4 and ch1 by 1
        radio1_ch += 2;
    }
    else
    { // If flag is not 0, decrement ch by 4 and ch1 by 1
        radio1_ch -= 2;
    }

    // Check if ch is greater than 79 and flag is 0
    if ((radio1_ch > MAX_CHANNEL) && (radio1_flag == 0))
    {
        radio1_flag = 1; // Set flag to 1 to change direction
    }
    else if ((radio1_ch < MIN_CHANNEL) && (radio1_flag == 1))
    {             // Check if ch is less than 2 and flag is 1
        radio1_flag = 0; // Set flag to 0 to change direction
    }


    /*
    if (flagv == 0)
    { // If flag is 0, increment ch by 4 and ch1 by 1

        ch1 += 4;
    }
    else
    { // If flag is not 0, decrement ch by 4 and ch1 by 1

        ch1 -= 4;
    }

    // Check if ch1 is greater than 79 and flag is 0
    if ((ch1 > MAX_CHANNEL) && (flagv == 0))
    {
        flagv = 1; // Set flag to 1 to change direction
    }
    else if ((ch1 < MIN_CHANNEL) && (flagv == 1))
    {              // Check if ch1 is less than 2 and flag is 1
        flagv = 0; // Set flag to 0 to change direction
    }
    */
    radio1.setChannel(radio1_ch);
    //Serial.print("SP:");
    //Serial.println(radio1_ch);
    /*
    Serial.print("\tHP:");
    Serial.println(radio2_ch);
    */
}

void randomChannelMode()
{
    ////RANDOM CHANNEL
    radio1.setChannel(random(86));
    //radio2.setChannel(random(80));
    /// delayMicroseconds(random(60));//////REMOVE IF SLOW

    /*  YOU CAN DO -----SWEEP CHANNEL
    for (int i = 0; i < 79; i++) {
    radio.setChannel(i);
    */
}

void initRadio1()
{
    vspi = new SPIClass(VSPI);
    vspi->begin();
    if (!radio1.begin(vspi)|| !radio1.isChipConnected())
    {
        Serial.println("VSPI NOT NOT STARTED!!!");
    }
    else
    {
        radio1.setAutoAck(false);
        radio1.stopListening();
        radio1.setRetries(0, 0);
        radio1.setPALevel(RF24_PA_MAX, true);
        radio1.setDataRate(RF24_2MBPS);
        radio1.setCRCLength(RF24_CRC_DISABLED);
        radio1.printPrettyDetails();
        radio1.startConstCarrier(RF24_PA_MAX, radio1_ch);
        Serial.println("VSPI STARTED!: \n");
        radio1.printPrettyDetails();
    }
}

/*
void initRadio2()
{
    vhspi = new SPIClass(HSPI);
    hspi->begin();
    if (!radio2.begin(hspi)|| !radio2.isChipConnected())
    {
        Serial.println("HSPI NOT NOT STARTED!!!");
    }
    else
    {
        radio1.setAutoAck(false);
        radio1.stopListening();
        radio1.setRetries(0, 0);
        radio1.setPALevel(RF24_PA_MAX, true);
        radio1.setDataRate(RF24_2MBPS);
        radio1.setCRCLength(RF24_CRC_DISABLED);
        radio1.printPrettyDetails();
        radio1.startConstCarrier(RF24_PA_MAX, radio2_ch);
        Serial.println("HSPI STARTED!:\n");
        radio1.printPrettyDetails();
    }
}
*/

void setup() {
  Serial.begin(115200);
  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_wifi_disconnect();


  pinMode(LED_PIN, OUTPUT);
  delay(1000);

  initRadio1();
  //initRadio2();

}

void loop() {
  goThroughChannelsMode();
}

