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

#include <Arduino.h>
#include <SPI.h>
#include "ADS131M04_CB.h"

ADS131M04_CB::ADS131M04_CB(int8_t _csPin, int8_t _drdyPin, SPIClass* _spi) {
  csPin = _csPin;
  drdyPin = _drdyPin;
  spi = _spi;
  initialised = false;
}

void ADS131M04_CB::begin(void) {
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  pinMode(drdyPin, INPUT);

  spi->begin();

  initialised=true;
}

void ADS131M04_CB::rawChannels(int8_t * channelArrPtr, int8_t channelArrLen, int32_t * outputArrPtr) {
  /* Writes data from the channels specified in channelArr, to outputArr,
     in the correct order.

     channelArr should have values from 0-3, and channelArrLen should be the
     length of that array, starting from 1.
  */
  
  uint32_t rawDataArr[6];

  // Get data
  spiCommFrame(&rawDataArr[0]);
  
  // Save the decoded data for each of the channels
  for (int8_t i = 0; i<channelArrLen; i++) {
    *outputArrPtr = twoCompDeco(rawDataArr[*channelArrPtr+1]);
    outputArrPtr++;
    channelArrPtr++;
  }
}

int32_t ADS131M04_CB::rawChannelSingle(int8_t channel) {
  /* Returns raw value from a single channel
     channel input from 0-3
  */
  
  int32_t outputArr[1];
  int8_t channelArr[1] = {channel};

  rawChannels(&channelArr[0], 1, &outputArr[0]);

  return outputArr[0];
}

bool ADS131M04_CB::globalChop(bool enabled, uint8_t log2delay) {
  /* Function to configure global chop mode for the ADS131M04.

     INPUTS:
     enabled - Whether to enable global-chop mode.
     log2delay   - Base 2 log of the desired delay in modulator clocks periods
     before measurment begins
     Possible values are between and including 1 and 16, to give delays
     between 2 and 65536 clock periods respectively
     For more information, refer to the datasheet.

     Returns true if settings were written succesfully.
  */

  uint8_t delayRegData = log2delay - 1;

  // Get current settings for current detect mode from the CFG register
  uint16_t currentDetSett = (readReg(ADS131M04_REG_CFG) << 8) >>8;
  
  uint16_t newRegData = (delayRegData << 12) + (enabled << 8) + currentDetSett;

  return writeReg(ADS131M04_REG_CFG, newRegData);
}

bool ADS131M04_CB::writeReg(uint8_t reg, uint16_t data) {
  /* Writes the content of data to the register reg
     Returns true if successful
  */
  
  uint8_t commandPref = 0x06;

  // Make command word using syntax found in data sheet
  uint16_t commandWord = (commandPref<<12) + (reg<<7);

  digitalWrite(csPin, LOW);
  spi->beginTransaction(SPISettings(SCLK_SPD, MSBFIRST, SPI_MODE1));

  spiTransferWord(commandWord);
  
  spiTransferWord(data);

  // Send 4 empty words
  for (uint8_t i=0; i<4; i++) {
    spiTransferWord();
  }

  spi->endTransaction();
  digitalWrite(csPin, HIGH);

  // Get response
  uint32_t responseArr[6];
  spiCommFrame(&responseArr[0]);

  if ( ( (0x04<<12) + (reg<<7) ) == responseArr[0]) {
    return true;
  } else {
    return false;
  }
}

bool ADS131M04_CB::setGain(uint8_t log2Gain0, uint8_t log2gainCommand, uint8_t log2Gain2, uint8_t log2Gain3) {
  /* Function to set the gain of the four channels of the ADC
     
     Inputs are the log base 2 of the desired gain to be applied to each
     channel.

     Returns true if gain was succesfully set.

     Function written by Iris Clercq-Roques
  */
  uint16_t gainCommand=log2Gain3<<4;
  gainCommand+=log2Gain2;
  gainCommand<<=8;
  gainCommand+=(log2gainCommand<<4);
  gainCommand+=log2Gain0;
  return writeReg(ADS131M04_REG_GAIN1, gainCommand);
}

uint16_t ADS131M04_CB::readReg(uint8_t reg) {
  /* Reads the content of single register found at address reg
     Returns register value
  */
  
  uint8_t commandPref = 0x0A;

  // Make command word using syntax found in data sheet
  uint16_t commandWord = (commandPref << 12) + (reg << 7);

  uint32_t responseArr[6];

  // Use first frame to send command
  spiCommFrame(&responseArr[0], commandWord);

  // Read response
  spiCommFrame(&responseArr[0]);

  return responseArr[0] >> 16;
}

/************************************************ 
    private functions
*************************************************/
uint32_t ADS131M04_CB::spiTransferWord(uint16_t inputData) {
  /* Transfer a 24 bit word
     Data returned is MSB aligned
  */ 

  uint32_t data = spi->transfer(inputData >> 8);
  data <<= 8;
  data |= spi->transfer((inputData<<8) >> 8);
  data <<= 8;
  data |= spi->transfer(0x00);

  return data << 8;
}

void ADS131M04_CB::spiCommFrame(uint32_t * outPtr, uint16_t command) {
  // Saves all the data of a communication frame to an array with pointer outPtr

  digitalWrite(csPin, LOW);

  spi->beginTransaction(SPISettings(SCLK_SPD, MSBFIRST, SPI_MODE1));

  // Send the command in the first word
  *outPtr = spiTransferWord(command);

  // For the next 4 words, just read the data
  for (uint8_t i=1; i < 5; i++) {
    outPtr++;
    *outPtr = spiTransferWord() >> 8;
  }

  // Save CRC bits
  outPtr++;
  *outPtr = spiTransferWord();

  spi->endTransaction();

  digitalWrite(csPin, HIGH);
}

int32_t ADS131M04_CB::twoCompDeco(uint32_t data) {
  // Take the two's complement of the data

  data <<= 8;
  int32_t dataInt = (int)data;

  return dataInt/pow(2,8);
}


