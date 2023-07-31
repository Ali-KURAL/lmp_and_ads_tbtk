#include <Arduino.h>
#include <SPI.h>
#include "lmp91050.h"
#include "ads131m04.h"
//Kırmızı -> VCC
//Beyaz-Siyah -> GND
//Siyah-Yeşil -> MOSI(MOSI_IN)
//Mavi -> SCK
//Sarı -> SDIO (LMP91050'nin biri, Diğeri CS2) (20)
//Turuncu -> CS0 (ADS131m04)  (17)
//Yeşil -> CS1
//Beyaz -> MISO

//1 vcc
//2 GND
//3 MOSI_IN
//4 SCK_IN
//5

using namespace ads131m04;

#define PIN_SPI_TX      19
#define PIN_SPI_RX      16
#define PIN_SPI_SCK     18

#define PIN_CS0_IN 17
#define PIN_CS1_IN 20
#define PIN_SDIO 15


#define WATCHDOG_TO     5000

lmp91050 lmp0;
lmp91050 lmp1;
//cADS131M04 gAds;

void setup() {
    pinMode(25,OUTPUT);
    Serial.begin(9600);
    pinMode(LED_BUILTIN,OUTPUT);
    SPI.setTX(PIN_SPI_TX);
    SPI.setRX(PIN_SPI_RX);
    SPI.setSCK(PIN_SPI_SCK);
    lmp0.begin(&SPI, PIN_SDIO ,PIN_CS0_IN, PIN_CS1_IN, false);
    lmp1.begin(&SPI, PIN_SDIO ,PIN_CS0_IN, PIN_CS1_IN, true);


    lmp0.writeDevConf(0x10);
    lmp0.writeDAC(0x10);

    lmp1.writeDevConf(0x30);
    lmp1.writeDAC(0x20);
    //gAds.begin(&SPI,ADS_CS);
    delay(5);
    lmp0.modeSDIO(true);
    lmp1.modeSDIO(true);
    delay(5);


/*
    bool begin_done = gAds.readID();

    if(!begin_done) {
        Serial.println("ads131m04 bağlanamadı");
    }else{
        Serial.println("ads131m04 bağlantı başarılı");
    }
    */
    //gAds.setGain(0,0,0,0);
    Serial.println("Configuration done");

//    watchdog_enable(WATCHDOG_TO, true);

}

void loop() {
//    watchdog_update();
    //Serial.println(lmp0.readDevConf());
    Serial.println(lmp1.readDAC());
    Serial.println(lmp0.readDAC());
    delay(300);
}