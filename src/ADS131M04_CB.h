/******************************************************************************
 *
 * This is a library/example for the 24 bit, 4 channel ADS131M04 A/D Converter
 *
 * This library is based on Imperial College London Rocketry
 * Created by Daniele Valentino Bella & Iris Clercq-Roques
 *
 * A number of changes were made including the use of an external OSC instead
 * of ESP32-generated PWM for clock.
 *
 * You are free to use it, change it or build on it. In case you like it, it would
 * be cool if you give it a star.
 *
 * If you find bugs, please inform me!
 *
 * Written by CoolBitts LLC
 * info@coolbitts.com
 *
 * ADC Product information: https://www.ti.com/product/ADS131M04
 * ADC Datasheet: https://www.ti.com/lit/gpn/ads131m04
 *
 ******************************************************************************/

#ifndef ADS131M04_CB_H_
#define ADS131M04_CB_H_

#define CLKIN_SPD 8192000 // CLKIN input frequency
#define SCLK_SPD 25000000 // SPI SCLK output frequency

#define ADS131M04_REG_ID            0x00
#define ADS131M04_REG_STATUS        0x01
#define ADS131M04_REG_MODE          0x02
#define ADS131M04_REG_CLOCK         0x03
#define ADS131M04_REG_GAIN1         0x04
#define ADS131M04_REG_CFG           0x06
#define ADS131M04_REG_THRSHLD_MSB   0x07
#define ADS131M04_REG_THRSHLD_LSB   0x08
#define ADS131M04_REG_CH0_CFG       0x09
#define ADS131M04_REG_CH0_OCAL_MSB  0x0A
#define ADS131M04_REG_CH0_OCAL_LSB  0x0B
#define ADS131M04_REG_CH0_GCAL_MSB  0x0C
#define ADS131M04_REG_CH0_GCAL_LSB  0x0D
#define ADS131M04_REG_CH1_CFG       0x0E
#define ADS131M04_REG_CH1_OCAL_MSB  0x0F
#define ADS131M04_REG_CH1_OCAL_LSB  0x10
#define ADS131M04_REG_CH1_GCAL_MSB  0x11
#define ADS131M04_REG_CH1_GCAL_LSB  0x12
#define ADS131M04_REG_CH2_CFG       0x13
#define ADS131M04_REG_CH2_OCAL_MSB  0x14
#define ADS131M04_REG_CH2_OCAL_LSB  0x15
#define ADS131M04_REG_CH2_GCAL_MSB  0x16
#define ADS131M04_REG_CH2_GCAL_LSB  0x17
#define ADS131M04_REG_CH3_CFG       0x18
#define ADS131M04_REG_CH3_OCAL_MSB  0x19
#define ADS131M04_REG_CH3_OCAL_LSB  0x1A
#define ADS131M04_REG_CH3_GCAL_MSB  0x1B
#define ADS131M04_REG_CH3_GCAL_LSB  0x1C
#define ADS131M04_REG_REGMAP_CRC    0x3E
#define ADS131M04_REG_RESERVED      0x3F

#include <Arduino.h>
#include <SPI.h>

class ADS131M04_CB {
  public:
    ADS131M04_CB(int8_t _csPin, int8_t _drdyPin, SPIClass* _spi);
    void begin(void);
    void rawChannels(int8_t * channelArrPtr, int8_t channelArrLen, int32_t * outputArrPtr);
    int32_t rawChannelSingle(int8_t channel);
    uint16_t readReg(uint8_t reg);
    bool writeReg(uint8_t reg, uint16_t data);
    bool setGain(uint8_t log2Gain0 = 0, uint8_t log2Gain1 = 0, uint8_t log2Gain2 = 0, uint8_t log2Gain3 = 0);
    bool globalChop(bool enabled = false, uint8_t log2delay = 4);

  private:
    int8_t csPin, drdyPin;
    SPIClass* spi;
    bool initialised;
    
    void spiCommFrame(uint32_t * outputArray, uint16_t command = 0x0000);
    uint32_t spiTransferWord(uint16_t inputData = 0x0000);
    int32_t twoCompDeco(uint32_t data);
};

#endif

