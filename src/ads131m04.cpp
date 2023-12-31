#include "ads131m04.h"
#include <SPI.h>

using namespace ads131m04;

bool cADS131M04::begin(SPIClass* pSpi, int8_t chipselect_dc_1,int8_t chipselect_dc_2,
                       int8_t clockOutPin, int8_t clockChannel)
{
    if (pSpi == nullptr)
    {
        // invalid parameter
        Serial.println("pSpi is NULL");
        return false;
    }

    this->m_chipSelectPin = chipselect_dc_1;
    this->m_chipSelectPin_2 = chipselect_dc_2;
    this->m_clockOutPin = clockOutPin;
    this->m_pSpi = pSpi;
    this->m_clockChannel = clockChannel;

    pinMode(this->m_chipSelectPin, OUTPUT);
    pinMode(this->m_chipSelectPin_2, OUTPUT);

    digitalWrite(this->m_chipSelectPin, HIGH);
    digitalWrite(this->m_chipSelectPin_2, HIGH);

    this->m_pSpi->begin();

    this->m_Initialized = true;

    bool result = this->readID();
    Serial.println(result);
    return result;
}

bool cADS131M04::readID()
{
    uint16_t idData = this->readRegister((std::uint8_t)READ_ONLY_REG::ID);

    if (idData != (std::uint16_t)readID::ID)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void cADS131M04::readChannels(int8_t *pChannel, int8_t nChannel, int32_t *pOutput)
{
    uint32_t rawData[6];

    // Get data
    this->spiCommFrame(&rawData[0]);

    // Save the decoded data for each of the channels
    for (int8_t i = 0; i < nChannel; i++)
    {
        *pOutput = this->twoComplement(rawData[*pChannel + 1]);
        pOutput++;
        pChannel++;
    }
}

int32_t cADS131M04::readSingleChannel(uint8_t channelNumber)
{
    int32_t pOutput[1];
    int8_t pChannel[1] = {static_cast<int8_t>(channelNumber)};

    this->readChannels(&pChannel[0], sizeof(pChannel), &pOutput[0]);

    return pOutput[0];
}

float cADS131M04::readVoltage(uint8_t channelNumber)
{
    int32_t code;
    float voltage;

    code = this->readSingleChannel(channelNumber);
    voltage = (code / (float)this->m_bits) * this->m_fsr;

    return voltage;
}

bool cADS131M04::setGain(uint8_t channelGain0, uint8_t channelGain1, uint8_t channelGain2, uint8_t channelGain3)
{
    uint16_t gainCommand;
    bool result;

    gainCommand = channelGain3 << 12;
    gainCommand = gainCommand | channelGain2;
    gainCommand = gainCommand << 8;
    gainCommand = gainCommand | channelGain1;
    gainCommand = gainCommand << 4;
    gainCommand = gainCommand | channelGain0;

    result = this->writeRegister((std::uint8_t)GBL_CH_SETTINGS_REG::GAIN, gainCommand);
    return result;
}

bool cADS131M04::globalChop(bool enable, uint8_t chopDelay)
{
    bool result;
    uint8_t delayData = chopDelay - 1;

    // Get current settings for current detect mode from the CFG register
    uint16_t currentSetting = (this->readRegister((std::uint8_t)GBL_CH_SETTINGS_REG::CONFIGURE) << 8) >>8;

    uint16_t newData = (delayData << 12) + (enable << 8) + currentSetting;

    result = this->writeRegister((std::uint8_t)GBL_CH_SETTINGS_REG::CONFIGURE, newData);
    return result;
}

uint16_t cADS131M04::readRegister(uint8_t registerAddr)
{
    uint16_t commandWord = (int)(Command::Read) | (registerAddr << 7) | 0;

    uint32_t pBuffer[6];

    // Use first frame to send command
    this->spiCommFrame(&pBuffer[0], commandWord);

    // Read response
    this->spiCommFrame(&pBuffer[0]);

    return pBuffer[0] >> 16;
}

bool cADS131M04::writeRegister(uint8_t registerAddr, uint16_t data)
{
    uint16_t commandWord = (int)(Command::Write) | (registerAddr << 7) | 0;

    digitalWrite(this->m_chipSelectPin, LOW);
    digitalWrite(this->m_chipSelectPin_2, LOW);
    this->m_pSpi->beginTransaction(SPISettings((std::uint32_t)SpeedSettings::SerialClock, MSBFIRST, (std::uint8_t)SPI_MODE1));

    this->spiTransferWord(commandWord);

    this->spiTransferWord(data);

    // Send 4 empty words
    for (uint8_t i = 0; i < 4; i++)
    {
        this->spiTransferWord();
    }

    this->m_pSpi->endTransaction();
    digitalWrite(this->m_chipSelectPin, HIGH);
    digitalWrite(this->m_chipSelectPin_2, HIGH);

    // Get response
    uint32_t pBuffer[6];
    this->spiCommFrame(&pBuffer[0]);

    if (((int)(CommandResponse::Write) | (registerAddr << 7) | 0) == pBuffer[0])
    {
        return true;
    }
    else
    {
        return false;
    }
}

void cADS131M04::spiCommFrame(uint32_t * pOutput, uint16_t command)
{
    digitalWrite(this->m_chipSelectPin, LOW);
    digitalWrite(this->m_chipSelectPin_2, LOW);

    this->m_pSpi->beginTransaction(SPISettings((std::uint32_t)SpeedSettings::SerialClock, MSBFIRST, (std::uint8_t)SPI_MODE1));

    // Send the command in the first word
    *pOutput = this->spiTransferWord(command);

    // For the next 4 words, just read the data
    for (uint8_t i = 1; i < 5; i++)
    {
        pOutput++;
        *pOutput = this->spiTransferWord() >> 8;
    }

    // Save CRC bits
    pOutput++;
    *pOutput = this->spiTransferWord();

    this->m_pSpi->endTransaction();

    digitalWrite(this->m_chipSelectPin, HIGH);
    digitalWrite(this->m_chipSelectPin_2, HIGH);
}

uint32_t cADS131M04::spiTransferWord(uint16_t inputData)
{
    uint32_t data = this->m_pSpi->transfer(inputData >> 8);
    data <<= 8;
    data |= this->m_pSpi->transfer((inputData<<8) >> 8);
    data <<= 8;
    data |= this->m_pSpi->transfer(0x00);

    return data << 8;
}

int32_t cADS131M04::twoComplement(uint32_t data)
{
    data <<= 8;
    int32_t intData = (int)data;

    return (int32_t)(intData / pow(2,8));
}