//
// Created by ali-k on 25.07.2023.
//

#include "lmp91050.h"

lmp91050::lmp91050() = default;


void lmp91050::begin(SPIClass *userSPI, uint8_t pin_SDIO, uint8_t userCS,  uint8_t userCS2, bool state) {
    this->_spi = userSPI;
    this->_spi->begin();
    this->_pin_SDIO = pin_SDIO;
    this->_cs = userCS;
    this->_cs2 = userCS2;
    this->_state = state;

    pinMode(pin_SDIO, OUTPUT);
    pinMode(userCS, OUTPUT);
    pinMode(userCS2, OUTPUT);

    digitalWrite(this->_cs,HIGH);
    digitalWrite(this->_cs2,HIGH);
}

uint8_t lmp91050::readRegister(uint8_t addr) {
    this->_spi->beginTransaction(SPISettings(LMP_SPI_FREQ, MSBFIRST, LMP_SPI_MODE));
    digitalWrite(this->_cs, !this->_state);
    digitalWrite(this->_cs2, _state);

    uint8_t opr = LMP91050_RW_BIT | addr;

    digitalWrite(this->_pin_SDIO, LOW);
    this->_spi->transfer(opr);

    digitalWrite(this->_pin_SDIO, HIGH);
   
    uint8_t response = this->_spi->transfer(0x00);

    digitalWrite(this->_cs, HIGH);
    digitalWrite(this->_cs2, HIGH);
    this->_spi->endTransaction();
    return response;
}

void lmp91050::writeRegister(uint8_t addr, uint8_t value) {
    this->_spi->beginTransaction(SPISettings(LMP_SPI_FREQ, MSBFIRST, LMP_SPI_MODE));
    digitalWrite(this->_cs, !this->_state);
    digitalWrite(this->_cs2, this->_state);
    uint8_t opr = (~LMP91050_RW_BIT) & addr;
    this->_spi->transfer(opr);
    this->_spi->transfer(value);
    digitalWrite(this->_cs, HIGH);
    digitalWrite(this->_cs2, HIGH);
    this->_spi->endTransaction();
}

void lmp91050::modeSDIO(bool state) {
    if (state){
        this->writeRegister(LMP91050_SDIO_MODE_ENABLE_REG,LMP91050_SDIO_SEQ_BYTE1);
        this->writeRegister(LMP91050_SDIO_MODE_ENABLE_REG,LMP91050_SDIO_SEQ_BYTE2);
    }else{
        this->writeRegister(LMP91050_SDIO_MODE_ENABLE_REG,0x00);
        this->writeRegister(LMP91050_SDIO_MODE_ENABLE_REG,0x00);
    }
}

void lmp91050::writeDevConf(uint8_t conf) {
    this->writeRegister(LMP91050_DEV_CONFIG_REG,conf);
}

void lmp91050::writeDAC(uint8_t value) {
    this->writeRegister(LMP91050_DAC_CONFIG_REG,value);
}

uint8_t lmp91050::readDevConf() {
    return this->readRegister(LMP91050_DEV_CONFIG_REG);
}

uint8_t lmp91050::readDAC() {
    return this->readRegister(LMP91050_DAC_CONFIG_REG);
}




