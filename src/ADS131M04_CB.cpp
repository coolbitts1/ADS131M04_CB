/*****************************************
* This is a library for the 24 bit, 4 channel ADS1220 A/D Converter
*
* I added example sketches which should enable you to use the library. 
*
* You are free to use it, change it or build on it. In case you like 
* it, it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
*
* You'll find a tutorial soon on:
*
* https://wolles-elektronikkiste.de/en/     (English)
* https://wolles-elektronikkiste.de/        (German)

*
*******************************************/

#include "ADS131M04_CB.h"

ADS131M04_CB::ADS131M04_CB(int cs, int drdy, SPIClass *s){
    csPin = cs;
    drdyPin = drdy;
    _spi = s;
}

uint8_t ADS131M04_CB::init(){
    vRef = 2.048;
    gain = 1; 
    refMeasurement = false; 
    convMode = ADS1220_SINGLE_SHOT;
    _spi->begin();
    pinMode(csPin, OUTPUT);
    pinMode(drdyPin, INPUT);
    digitalWrite(csPin, HIGH);
    mySPISettings = SPISettings(4000000, MSBFIRST, SPI_MODE1); 
    reset();
    start();
    uint8_t ctrlVal = 0;
    bypassPGA(true); // just a test if the ADS1220 is connected
    ctrlVal = readRegister(ADS1220_CONF_REG_0);
    ctrlVal = ctrlVal & 0x01;
    bypassPGA(false);
    return ctrlVal; 
}

void ADS131M04_CB::start(){
    command(ADS1220_START);
}

void ADS131M04_CB::reset(){
    command(ADS1220_RESET);
    delay(1);
}

void ADS131M04_CB::powerDown(){
    command(ADS1220_PWRDOWN);
}

/* Configuration Register 0 settings */

void ADS131M04_CB::setCompareChannels(ads1220Mux mux){
    if((mux == ADS1220_MUX_REFPX_REFNX_4) || (mux == ADS1220_MUX_AVDD_M_AVSS_4)){
        gain = 1;    // under these conditions gain is one by definition 
        refMeasurement = true; 
    }
    else{            // otherwise read gain from register
        regValue = readRegister(ADS1220_CONF_REG_0);
        regValue = regValue & 0x0E;
        regValue = regValue>>1;
        gain = 1 << regValue;
        refMeasurement = false;
    }
    regValue = readRegister(ADS1220_CONF_REG_0);
    regValue &= ~0xF1;
    regValue |= mux;
    regValue |= !(doNotBypassPgaIfPossible & 0x01);
    writeRegister(ADS1220_CONF_REG_0, regValue);
    if((mux >= 0x80) && (mux <=0xD0)){
        if(gain > 4){
            gain = 4;           // max gain is 4 if single-ended input is chosen or PGA is bypassed
        }
        forcedBypassPGA();
    }
}

void ADS131M04_CB::setGain(ads1220Gain enumGain){
    regValue = readRegister(ADS1220_CONF_REG_0);
    ads1220Mux mux = (ads1220Mux)(regValue & 0xF0);
    regValue &= ~0x0E;
    regValue |= enumGain;
    writeRegister(ADS1220_CONF_REG_0, regValue);
    
    gain = 1<<(enumGain>>1);
    if((mux >= 0x80) && (mux <=0xD0)){
        if(gain > 4){
            gain = 4;   // max gain is 4 if single-ended input is chosen or PGA is bypassed
        }
        forcedBypassPGA();
    }
}

uint8_t ADS131M04_CB::getGainFactor(){
    return gain;
}

void ADS131M04_CB::bypassPGA(bool bypass){
    regValue = readRegister(ADS1220_CONF_REG_0);
    regValue &= ~0x01;
    regValue |= bypass;
    doNotBypassPgaIfPossible = !(bypass & 0x01);
    writeRegister(ADS1220_CONF_REG_0, regValue);
}

bool ADS131M04_CB::isPGABypassed(){
    regValue = readRegister(ADS1220_CONF_REG_0);
    return regValue & 0x01;
}

/* Configuration Register 1 settings */

void ADS131M04_CB::setDataRate(ads1220DataRate rate){
    regValue = readRegister(ADS1220_CONF_REG_1);
    regValue &= ~0xE0;
    regValue |= rate;
    writeRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS131M04_CB::setOperatingMode(ads1220OpMode mode){
    regValue = readRegister(ADS1220_CONF_REG_1);
    regValue &= ~0x18;
    regValue |= mode;
    writeRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS131M04_CB::setConversionMode(ads1220ConvMode mode){
    convMode = mode;
    regValue = readRegister(ADS1220_CONF_REG_1);
    regValue &= ~0x04;
    regValue |= mode;
    writeRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS131M04_CB::enableTemperatureSensor(bool enable){
    regValue = readRegister(ADS1220_CONF_REG_1);
    if(enable){
        regValue |= 0x02;
    }
    else{
        regValue &= ~0x02;
    }
    writeRegister(ADS1220_CONF_REG_1, regValue);
}

void ADS131M04_CB::enableBurnOutCurrentSources(bool enable){
    regValue = readRegister(ADS1220_CONF_REG_1);
    if(enable){
        regValue |= 0x01;
    }
    else{
        regValue &= ~0x01;
    }
    writeRegister(ADS1220_CONF_REG_1, regValue);
}

/* Configuration Register 2 settings */

void ADS131M04_CB::setVRefSource(ads1220VRef vRefSource){
    regValue = readRegister(ADS1220_CONF_REG_2);
    regValue &= ~0xC0;
    regValue |= vRefSource;
    writeRegister(ADS1220_CONF_REG_2, regValue);
}

void ADS131M04_CB::setFIRFilter(ads1220FIR fir){
    regValue = readRegister(ADS1220_CONF_REG_2);
    regValue &= ~0x30;
    regValue |= fir;
    writeRegister(ADS1220_CONF_REG_2, regValue);
}

void ADS131M04_CB::setLowSidePowerSwitch(ads1220PSW psw){
    regValue = readRegister(ADS1220_CONF_REG_2);
    regValue &= ~0x08;
    regValue |= psw;
    writeRegister(ADS1220_CONF_REG_2, regValue);
}

void ADS131M04_CB::setIdacCurrent(ads1220IdacCurrent current){
    regValue = readRegister(ADS1220_CONF_REG_2);
    regValue &= ~0x07;
    regValue |= current;
    writeRegister(ADS1220_CONF_REG_2, regValue);
    delayMicroseconds(200);
}

/* Configuration Register 3 settings */

void ADS131M04_CB::setIdac1Routing(ads1220IdacRouting route){
    regValue = readRegister(ADS1220_CONF_REG_3);
    regValue &= ~0xE0;
    regValue |= (route<<5);
    writeRegister(ADS1220_CONF_REG_3, regValue);
}

void ADS131M04_CB::setIdac2Routing(ads1220IdacRouting route){
    regValue = readRegister(ADS1220_CONF_REG_3);
    regValue &= ~0x1C;
    regValue |= (route<<2);
    writeRegister(ADS1220_CONF_REG_3, regValue);
}

void ADS131M04_CB::setDrdyMode(ads1220DrdyMode mode){
    regValue = readRegister(ADS1220_CONF_REG_3);
    regValue &= ~0x02;
    regValue |= mode;
    writeRegister(ADS1220_CONF_REG_3, regValue);
}

/* Other settings */

void ADS131M04_CB::setSPIClockSpeed(unsigned long clock){
    mySPISettings = SPISettings(clock, MSBFIRST, SPI_MODE1);
}

void ADS131M04_CB::setVRefValue_V(float refVal){
    vRef = refVal;
}

float ADS131M04_CB::getVRef_V(){
    return vRef;
}

void ADS131M04_CB::setAvddAvssAsVrefAndCalibrate(){
    float avssVoltage = 0.0;
    setVRefSource(ADS1220_VREF_AVDD_AVSS);  
    setCompareChannels(ADS1220_MUX_AVDD_M_AVSS_4);
    for(int i = 0; i<10; i++){
        avssVoltage += getVoltage_mV();
    }
    vRef = avssVoltage * 4.0 / 10000.0; 
}

void ADS131M04_CB::setRefp0Refn0AsVefAndCalibrate(){
    float ref0Voltage = 0.0;
    setVRefSource(ADS1220_VREF_REFP0_REFN0);
    setCompareChannels(ADS1220_MUX_REFPX_REFNX_4);
    for(int i = 0; i<10; i++){
        ref0Voltage += getVoltage_mV();
    }
    vRef = ref0Voltage * 4.0 / 10000.0; 
}

void ADS131M04_CB::setRefp1Refn1AsVefAndCalibrate(){
    float ref1Voltage = 0.0;
    setVRefSource(ADS1220_VREF_REFP1_REFN1);
    setCompareChannels(ADS1220_MUX_REFPX_REFNX_4);
    for(int i = 0; i<10; i++){
        ref1Voltage += getVoltage_mV();
    }
    vRef = ref1Voltage * 4.0 / 10000.0; 
}

void ADS131M04_CB::setIntVRef(){
    setVRefSource(ADS1220_VREF_REFP1_REFN1);
    vRef = 2.048;
}
    

/* Results */
float ADS131M04_CB::getVoltage_mV(){
    int32_t rawData = getData();
    float resultInMV = 0.0;
    if(refMeasurement){
        resultInMV = (rawData / ADS1220_RANGE) * 2.048 * 1000.0 / (gain * 1.0);
    }
    else{
        resultInMV = (rawData / ADS1220_RANGE) * vRef * 1000.0 / (gain * 1.0);
    }
    return resultInMV;
}

float ADS131M04_CB::getVoltage_muV(){
    return getVoltage_mV() * 1000.0;
}

int32_t ADS131M04_CB::getRawData(){
    return getData();
}

float ADS131M04_CB::getTemperature(){
    enableTemperatureSensor(true);
    uint32_t rawResult = readResult();
    enableTemperatureSensor(false);
    
    uint16_t result = (uint16_t)(rawResult >> 18);
    if(result>>13){
        result = ~(result-1) & 0x3777;
        return result * (-0.03125);
    }
  
    return result * 0.03125;
}

/************************************************ 
    private functions
*************************************************/

void ADS131M04_CB::forcedBypassPGA(){
    regValue = readRegister(ADS1220_CONF_REG_0);
    regValue |= 0x01;
    writeRegister(ADS1220_CONF_REG_0, regValue);
}

int32_t ADS131M04_CB::getData(){
    uint32_t rawResult = readResult();
    int32_t result = ((int32_t)(rawResult)) >> 8;
    
    return result;
}

uint32_t ADS131M04_CB::readResult(){
    uint8_t buf[3];
    uint32_t rawResult = 0;

    if(convMode == ADS1220_SINGLE_SHOT){
        start();
    }
    while(digitalRead(drdyPin) == HIGH) {}           
    
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    buf[0] = _spi->transfer(0x00);
    buf[1] = _spi->transfer(0x00);
    buf[2] = _spi->transfer(0x00);
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    
    rawResult = buf[0];
    rawResult = (rawResult << 8) | buf[1];
    rawResult = (rawResult << 8) | buf[2];
    rawResult = (rawResult << 8);
    
    return rawResult;
}

uint8_t ADS131M04_CB::readRegister(uint8_t reg){
    regValue = 0;
    
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(ADS1220_RREG | (reg<<2)); 
    regValue = _spi->transfer(0x00);
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    
    return regValue;
}
   
void ADS131M04_CB::writeRegister(uint8_t reg, uint8_t val){
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(ADS1220_WREG | (reg<<2)); 
    _spi->transfer(val);
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
}

void ADS131M04_CB::command(uint8_t cmd){
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    SPI.transfer(cmd);
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
}   

