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
 * You are free to use it, change it or build on it. 
 * If you find bugs, please inform me!
 *
 * Written by CoolBitts LLC
 * info@coolbitts.com
 *
 * ADC Product information: https://www.ti.com/product/ADS131M04
 * ADC Datasheet: https://www.ti.com/lit/gpn/ads131m04
 *
 ******************************************************************************/

#include "ADS131M04_CB.h"
#include <Arduino.h>
#include <SPI.h>

// ----- ASSIGN SPI Interface PINS -------
#define XSPI_MISO   25
#define XSPI_MOSI   26
#define XSPI_SCLK   21
#define XSPI_SS     4
#define ADC_DRDY_L  35  // IN:  ADC DRDY*

SPIClass ADCSPI=SPIClass(VSPI);
ADS131M04_CB ADC=ADS131M04_CB(XSPI_SS, ADC_DRDY_L, &ADCSPI);

void printData() {

  int32_t outputArray[4];
  
  int8_t channelArr[4] = {0, 1, 2, 3};

  ADC.rawChannels(&channelArr[0], 4, &outputArray[0]);

  for (int8_t i = 0; i<4; i++) {
    Serial.print(outputArray[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
}
  /*
  uint32_t outputArray[6];
  ADC.spiCommFrame(&outputArray[0]);


  //Serial.println(outputArray[3]);

  //Serial.print(" ");

  Serial.print(outputArray[0]);
  Serial.print(" ");

  for (uint8_t i=1; i<5; i++) {
    Serial.print(ADC.twoCompDeco(outputArray[i]));
    Serial.print(" ");
  }
  Serial.print(outputArray[5]);
  Serial.print(" ");
  Serial.println();
}
*/

void setup() {
  Serial.begin(115200);

  //Initialise hardware SPI
  ADCSPI.begin(XSPI_SCLK, XSPI_MISO, XSPI_MOSI, XSPI_SS);

  ADC.begin();
  Serial.println("Initialised!");

  /*

  // Try to read every single register
  for (uint8_t i=0x00; i<=0x3F; i++) {
    Serial.print("Result for: ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(ADC.readReg(i));
  }
  
  // Try putting to sleep and waking it up
  printData();
  printData();
  printData();
  
  Serial.println("Putting to sleep...");
  ADC.sendCommand(0x0022);

  printData();
  printData();
  printData();

  Serial.println("Waking up... ");
  ADC.sendCommand(0x0033);
  printData();
  printData();
  printData();
  */
}

void loop() {
  /*
  printData();

  delay(100);
*/
//  printData();
  // Try writing the gain; and then reading that register
  printData();
  printData();
  printData();
/*
  Serial.print("Writing: ");
  Serial.println(ADC.setGain(3,3,3,3));
  /*
  Serial.println(ADC.writeReg(0x04,0x3333));
  Serial.print("Reading: ");
  Serial.println(ADC.readReg(0x04));
  Serial.println("#######################");

  delay(1000);

  printData();
  printData();
  printData();

  Serial.print("Writing: ");
  Serial.println(ADC.setGain());
  */
  /*
  Serial.println(ADC.writeReg(0x04,0x0000));
  Serial.print("Reading: ");
  Serial.println(ADC.readReg(0x04));
  Serial.println("#######################");
*/
  //delay(1000);
  // Try putting to sleep and waking it up
  /*
  printData();
  printData();
  printData();
  Serial.println("Putting to sleep...");
  ADC.sendCommand(0x0022);
  printData();
  printData();
  printData();

  Serial.println("Waking up... ");
  ADC.sendCommand(0x0033);
  printData();
  printData();
  printData();
  */
}