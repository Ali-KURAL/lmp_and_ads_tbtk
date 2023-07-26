#include <Arduino.h>
#include <SPI.h>
#include "lmp91050.h"
#include "ads131m04.h"

using namespace ads131m04;

#define PIN_SPI_TX      19
#define PIN_SPI_RX      16
#define PIN_SPI_SCK     18

#define PIN_SPI_CS      17
#define ADS_CS          17

#define WATCHDOG_TO     5000

lmp91050 lmp;
cADS131M04 gAds;

void setup() {
    pinMode(25,OUTPUT);
    Serial.begin(9600);
    pinMode(LED_BUILTIN,OUTPUT);
    SPI.setTX(PIN_SPI_TX);
    SPI.setRX(PIN_SPI_RX);
    SPI.setSCK(PIN_SPI_SCK);
    lmp.begin(&SPI,PIN_SPI_CS);
    gAds.begin(&SPI,ADS_CS);
    delay(5);
    lmp.writeDevConf(LMP_CONFIG_DEFAULT);
    lmp.writeDAC(25);
    delay(5);

    bool begin_done = gAds.readID();

    if(!begin_done) {
        Serial.println("ads131m04 bağlanamadı");
    }else{
        Serial.println("ads131m04 bağlantı başarılı");
    }
    gAds.setGain(0,0,0,0);
    Serial.println("Configuration done");

//    watchdog_enable(WATCHDOG_TO, true);

}

void loop() {
//    watchdog_update();
    digitalWrite(25,HIGH);
    delay(500);
    digitalWrite(25,LOW);
    delay(500);
}