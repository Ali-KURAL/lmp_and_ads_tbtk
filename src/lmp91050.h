//
// Created by ali-k on 26.07.2023.
//

#ifndef LMP_AND_ADS_LMP91050_H
#define LMP_AND_ADS_LMP91050_H

#include "SPI.h"

#define LMP_SPI_MODE            arduino::SPI_MODE2                             // Can be 3 0 1
#define LMP_SPI_FREQ            10000000

#define LMP91050_DEV_CONFIG_REG                         (0x00)                  // Device Configuration
#define LMP91050_DAC_CONFIG_REG                         (0x01)                  // DAC Configuration
#define LMP91050_SDIO_MODE_ENABLE_REG                   (0x0F)                  // SDIO Mode Enable

#define LMP91050_RW_BIT                                 (0x80)                  // Read Write Bit
#define LMP91050_SDIO_SEQ_BYTE1                         (0xFE)                  // Byte1 to write to enter SDIO Mode
#define LMP91050_SDIO_SEQ_BYTE2                         (0xED)                  // Byte2 to write to enter SDIO Mode

// PGA1 ON, PGA2 ON, external filter, 1.15V, gain2 = 4, gain1 = 250 (01110000)
#define LMP_CONFIG_DEFAULT      0x70
// https://www.ti.com/lit/ug/snau119/snau119.pdf?ts=1690356946493&ref_url=https%253A%252F%252Fwww.ti.com%252Ftool%252FLMP91050SDEVAL
class lmp91050 {
private:
    SPIClass * _spi{};
    uint8_t _cs{};
    void writeRegister(uint8_t addr, uint8_t value);
    uint8_t readRegister(uint8_t addr);
public:
    lmp91050();         // Constructor of the object
    void begin(SPIClass *userSPI,uint8_t userCS);       // starter function ( SPI class , CS pin )
    void modeSDIO(bool state); // SDIO --> open read mode (True)
    void writeDevConf(uint8_t conf); // Write device config 8 bit 00000000
    uint8_t readDevConf();
    void writeDAC(uint8_t value); // Write Dac config
    uint8_t readDAC();
};


// https://www.ti.com/lit/ds/symlink/lmp91050.pdf?ts=1690356904375&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FLMP91050
// P16-17-18

#endif //LMP_AND_ADS_LMP91050_H
