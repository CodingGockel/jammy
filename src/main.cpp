#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "esp_bt.h"
#include "esp_wifi.h"
#include <SPI.h>

SPIClass *vspi = nullptr;

// VSPI for RADIO 1
const uint8_t VSPI_CE_PIN = 4;
const uint8_t VSPI_CSN_PIN = 5;
const uint8_t VSPI_SCK = 18;
const uint8_t VSPI_MISO = 19;
const uint8_t VSPI_MOSI = 23;

//constants
const uint8_t LED_PIN = 2;
const uint8_t SPI_SPEED = 16000000;
const uint8_t MIN_CHANNEL = 2;
const uint8_t MAX_CHANNEL = 86;

uint8_t radio1_flag = 0;  // VSPI// Flag variable to keep track of direction
uint8_t radio1_ch = 45;            // Variable to store value of ch


// radio1 on VSPI
RF24 radio1(VSPI_CE_PIN, VSPI_CSN_PIN, SPI_SPEED);

void waveThroughCHannelsMode(const uint8_t MIN, const uint8_t MAX)
{

    if (radio1_flag == 0){ 
        radio1_ch += 1;
    }
    else{ 
        radio1_ch -= 1;
    }

    if ((radio1_ch >= MAX) && (radio1_flag == 0)){
        radio1_flag = 1; 
    }
    else if ((radio1_ch <= MIN) && (radio1_flag == 1)){          
        radio1_flag = 0;
    }

    radio1.setChannel(radio1_ch);
}

void iterateChannels(const uint8_t MIN, const uint8_t MAX, const uint8_t STEPS){
  for (uint8_t i = MIN; i <= MAX; i = i + STEPS) {
      radio1.setChannel(i); 
    }
}

void randomChannelMode(const uint8_t MAX)
{
    radio1.setChannel(random(MAX));
}

void initRadio1()
{
    vspi = new SPIClass(VSPI);
    vspi->begin();
    if (!radio1.begin(vspi)|| !radio1.isChipConnected()){
        Serial.println("VSPI NOT STARTED!!!");
    }
    else{
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

void setup() {
  Serial.begin(115200);
  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_wifi_disconnect();


  pinMode(LED_PIN, OUTPUT);
  delay(1000);

  initRadio1();

}

void loop(void) {
  iterateChannels(0, 86, 2); //BL RANGE
}

