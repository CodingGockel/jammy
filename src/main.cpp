#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "esp_bt.h"
#include "esp_wifi.h"
#include <SPI.h>
#include <SevSeg.h>

SevSeg sevseg;

SPIClass *vspi = nullptr;

// VSPI for RADIO 1
const uint8_t VSPI_CE_PIN = 4;
const uint8_t VSPI_CSN_PIN = 5;
const uint8_t VSPI_SCK = 18;
const uint8_t VSPI_MISO = 19;
const uint8_t VSPI_MOSI = 23;

//Button pins
const uint8_t BUTTON_UP = 13;
const uint8_t BUTTON_CLICK = 12;
const uint8_t BUTTON_DOWN = 14;
bool BUTTON_UP_PRESSED = false;
bool BUTTON_CLICK_PRESSED = false;
bool BUTTON_DOWN_PRESSED = false;

// Display ports
byte numDigits   = 4;
byte digitPins[] = {27, 33, 32, 21};        // D1..D4
byte segPins[]   = {26, 22, 0, 17, 2, 25, 15, 16}; // a,b,c,d,e,f,g,dp
bool resistorsOnSegments = true;            

//constants
const uint8_t LED_PIN = 2;
const int SPI_SPEED = 16000000;
const uint8_t MIN_CHANNEL = 2;
const uint8_t MAX_CHANNEL = 86;
int currentCount = 8888;
const int MAX_COUNT = 9999;
const int MIN_COUNT = 0;
bool vspiStarted = false;

uint8_t radio1_flag = 0;  // VSPI// Flag variable to keep track of direction
uint8_t radio1_ch = 45;   // Variable to store value of ch

// radio1 on VSPI
RF24 radio1(VSPI_CE_PIN, VSPI_CSN_PIN, SPI_SPEED);

void calculateCurrentCount() {

  if (digitalRead(BUTTON_UP) == LOW && !BUTTON_UP_PRESSED) {
    if (currentCount + 1 <= MAX_COUNT) currentCount += 1;
    BUTTON_UP_PRESSED = true;
  }
  if (digitalRead(BUTTON_UP) == HIGH) BUTTON_UP_PRESSED = false;

  if (digitalRead(BUTTON_DOWN) == LOW && !BUTTON_DOWN_PRESSED) {
    if (currentCount - 1 >= MIN_COUNT) currentCount -= 1;
    BUTTON_DOWN_PRESSED = true;
  }
  if (digitalRead(BUTTON_DOWN) == HIGH) BUTTON_DOWN_PRESSED = false;

  if (digitalRead(BUTTON_CLICK) == LOW && !BUTTON_CLICK_PRESSED) {
    if (currentCount + 1000 <= MAX_COUNT) currentCount += 1000;
    BUTTON_CLICK_PRESSED = true;
  }
  if (digitalRead(BUTTON_CLICK) == HIGH) BUTTON_CLICK_PRESSED = false;
}

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
        vspiStarted = true;
    }
}

void setup() {
  Serial.begin(115200);
  esp_bt_controller_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_wifi_disconnect();

  delay(1000);

  //SETUP BUTTONS
  pinMode(BUTTON_UP,    INPUT_PULLUP);
  pinMode(BUTTON_CLICK, INPUT_PULLUP);
  pinMode(BUTTON_DOWN,  INPUT_PULLUP);

  //SETUP RADIOS
  initRadio1();

  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segPins, resistorsOnSegments);
  sevseg.setBrightness(90);
}

void loop(void) {
  if(vspiStarted){
    iterateChannels(0, 86, 2); //BL RANGE
  }else{
    initRadio1();
  }
  
  calculateCurrentCount();
  sevseg.setNumber(currentCount, 0);
  sevseg.refreshDisplay();
}