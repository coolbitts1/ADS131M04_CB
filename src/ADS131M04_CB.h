/******************************************************************************
 *
 * This is a library for the 24 bit, 4 channel ADS131M04 A/D Converter
 *
 * I added several examples sketches which should enable you to use the library.
 *
 * You are free to use it, change it or build on it. In case you like it, it would
 * be cool if you give it a star.
 *
 * If you find bugs, please inform me!
 *
 * Written by CoolBitts LLC
 * info@coolbitts.com
 *
 ******************************************************************************/

#ifndef ADS131M04_CB_H_
#define ADS131M04_CB_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include<SPI.h>

//ADS131M04 SPI commands
#define ADS131M04_RESET       0x06    
#define ADS131M04_START       0x08    //Send the START/SYNC command (08h) to start converting in continuous conversion mode
#define ADS131M04_PWRDOWN     0x02
#define ADS131M04_RDATA       0x10
#define ADS131M04_WREG        0x40    // write register
#define ADS131M04_RREG        0x20    // read register

/* registers */
#define ADS131M04_CONF_REG_0  0x00
#define ADS131M04_CONF_REG_1  0x01
#define ADS131M04_CONF_REG_2  0x02
#define ADS131M04_CONF_REG_3  0x03

/* other */
#define ADS131M04_RANGE 8388607.0 // = 2^23 - 1

typedef enum ADS131M04_MUX {
    ADS131M04_MUX_0_1     = 0x00,   //default
    ADS131M04_MUX_0_2     = 0x10,
    ADS131M04_MUX_0_3     = 0x20,
    ADS131M04_MUX_1_2     = 0x30,
    ADS131M04_MUX_1_3     = 0x40,
    ADS131M04_MUX_2_3     = 0x50,
    ADS131M04_MUX_1_0     = 0x60,
    ADS131M04_MUX_3_2     = 0x70,
    ADS131M04_MUX_0_AVSS  = 0x80,
    ADS131M04_MUX_1_AVSS  = 0x90,
    ADS131M04_MUX_2_AVSS  = 0xA0,
    ADS131M04_MUX_3_AVSS  = 0xB0,
    ADS131M04_MUX_REFPX_REFNX_4 = 0xC0,
    ADS131M04_MUX_AVDD_M_AVSS_4 = 0xD0,
    ADS131M04_MUX_AVDD_P_AVSS_2 = 0xE0
} ADS131M04Mux;

typedef enum ADS131M04_GAIN {
    ADS131M04_GAIN_1   = 0x00,   //default
    ADS131M04_GAIN_2   = 0x02,
    ADS131M04_GAIN_4   = 0x04,
    ADS131M04_GAIN_8   = 0x06,
    ADS131M04_GAIN_16  = 0x08,
    ADS131M04_GAIN_32  = 0x0A,
    ADS131M04_GAIN_64  = 0x0C,
    ADS131M04_GAIN_128 = 0x0E
} ADS131M04Gain;

typedef enum ADS131M04_DATA_RATE {
    ADS131M04_DR_LVL_0 = 0x00,   // default
    ADS131M04_DR_LVL_1 = 0x20,
    ADS131M04_DR_LVL_2 = 0x40,
    ADS131M04_DR_LVL_3 = 0x60,
    ADS131M04_DR_LVL_4 = 0x80,
    ADS131M04_DR_LVL_5 = 0xA0,
    ADS131M04_DR_LVL_6 = 0xC0
} ADS131M04DataRate;

typedef enum ADS131M04_OP_MODE {      
    ADS131M04_NORMAL_MODE     = 0x00,  // default
    ADS131M04_DUTY_CYCLE_MODE = 0x08,
    ADS131M04_TURBO_MODE      = 0x10
} ADS131M04OpMode;

typedef enum ADS131M04_CONV_MODE {
    ADS131M04_SINGLE_SHOT     = 0x00,  // default
    ADS131M04_CONTINUOUS      = 0x04
} ADS131M04ConvMode;

typedef enum ADS131M04_VREF{
    ADS131M04_VREF_INT            = 0x00,  // default
    ADS131M04_VREF_REFP0_REFN0    = 0x40,
    ADS131M04_VREF_REFP1_REFN1    = 0x80,
    ADS131M04_VREF_AVDD_AVSS      = 0xC0
} ADS131M04VRef;

typedef enum ADS131M04_FIR{
    ADS131M04_NONE        = 0x00,   // default
    ADS131M04_50HZ_60HZ   = 0x10,
    ADS131M04_50HZ        = 0x20,
    ADS131M04_60HZ        = 0x30
} ADS131M04FIR;

typedef enum ADS131M04_PSW {
    ADS131M04_ALWAYS_OPEN = 0x00,  // default
    ADS131M04_SWITCH      = 0x08
} ADS131M04PSW;

typedef enum ADS131M04_IDAC_CURRENT {
    ADS131M04_IDAC_OFF        = 0x00,  // defaulr
    ADS131M04_IDAC_10_MU_A    = 0x01,
    ADS131M04_IDAC_50_MU_A    = 0x02,
    ADS131M04_IDAC_100_MU_A   = 0x03,
    ADS131M04_IDAC_250_MU_A   = 0x04,
    ADS131M04_IDAC_500_MU_A   = 0x05,
    ADS131M04_IDAC_1000_MU_A  = 0x06,
    ADS131M04_IDAC_1500_MU_A  = 0x07
} ADS131M04IdacCurrent;

typedef enum ADS131M04_IDAC_ROUTING {
    ADS131M04_IDAC_NONE       = 0x00,  // default
    ADS131M04_IDAC_AIN0_REFP1 = 0x01,
    ADS131M04_IDAC_AIN1       = 0x02,
    ADS131M04_IDAC_AIN2       = 0x03,
    ADS131M04_IDAC_AIN3_REFN1 = 0x04,
    ADS131M04_IDAC_REFP0      = 0x05,
    ADS131M04_IDAC_REFN0      = 0x06,
} ADS131M04IdacRouting;

typedef enum ADS131M04_DRDY_MODE {
    ADS131M04_DRDY      = 0x00,   // default
    ADS131M04_DOUT_DRDY = 0x02
} ADS131M04DrdyMode;

/* other */

class ADS131M04_CB
{
public:
    ADS131M04_CB(int cs, int drdy, SPIClass *s = &SPI);
    
    /* Commands */
    uint8_t init();
    void start();
    void reset();
    void powerDown();
    
    /* Configuration Register 0 settings */
    void setCompareChannels(ADS131M04Mux mux);
    void setGain(ADS131M04Gain gain);
    uint8_t getGainFactor();
    void bypassPGA(bool bypass);
    bool isPGABypassed();
    
    /* Configuration Register 1 settings */
    void setDataRate(ADS131M04DataRate rate);
    void setOperatingMode(ADS131M04OpMode mode);
    void setConversionMode(ADS131M04ConvMode mode);
    void enableTemperatureSensor(bool enable);
    void enableBurnOutCurrentSources(bool enable);
    
    /* Configuration Register 2 settings */
    void setVRefSource(ADS131M04VRef vRefSource);
    void setFIRFilter(ADS131M04FIR fir);
    void setLowSidePowerSwitch(ADS131M04PSW psw);
    void setIdacCurrent(ADS131M04IdacCurrent current);
    
    /* Configuration Register 3 settings */
    void setIdac1Routing(ADS131M04IdacRouting route);
    void setIdac2Routing(ADS131M04IdacRouting route);
    void setDrdyMode(ADS131M04DrdyMode mode);
    
    /* Other settings */
    void setSPIClockSpeed(unsigned long clock);
    void setVRefValue_V(float refVal);
    float getVRef_V();
    void setAvddAvssAsVrefAndCalibrate();
    void setRefp0Refn0AsVefAndCalibrate();
    void setRefp1Refn1AsVefAndCalibrate();
    void setIntVRef();
    
    /* Results */
    float getVoltage_mV();
    float getVoltage_muV();
    int32_t getRawData();
    float getTemperature();

private:
    SPIClass *_spi;
    SPISettings mySPISettings;
    int16_t csPin;
    int16_t drdyPin;
    uint8_t regValue;
    float vRef;
    uint8_t gain;
    bool refMeasurement; 
    bool doNotBypassPgaIfPossible;
    ADS131M04ConvMode convMode;

    void forcedBypassPGA();
    int32_t getData();
    uint32_t readResult();
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t val);
    void command(uint8_t cmd);
};

#endif

