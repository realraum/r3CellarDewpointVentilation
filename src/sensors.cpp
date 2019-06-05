#include <Arduino.h>
#include "sensors.h"
#include "config.h"
#include <Wire.h>
#include "SparkFunBME280.h"

GlobalSensorData sensors_;


GlobalSensorData const *getSensorData()
{
  return &sensors_;
}


TwoWire I2Ctwo = TwoWire(1);
// TwoWire I2Cthree = TwoWire(0);


//Define sensor objects
BME280 sensorIn;
BME280 sensorOut;

void initSensors(void)
{
  bzero(&sensors_,sizeof(GlobalSensorData));
  //Initialize Sensors
  // sensorIn.settings.commInterface = SPI_MODE;
  // sensorIn.settings.chipSelectPin = SPI_BMP_CS;
  sensorIn.settings.commInterface = I2C_MODE;
  sensorIn.settings.I2CAddress = 0x77;


  // sensorOut.settings.commInterface = SPI_MODE;
  // sensorOut.settings.chipSelectPin = 25;
  sensorOut.settings.commInterface = I2C_MODE;
  sensorOut.settings.I2CAddress = 0x76;

  sensorIn.settings.runMode = 3; //Normal mode
  sensorOut.settings.runMode = 3; //Normal mode

  sensorIn.settings.tStandby = 0;
  sensorOut.settings.tStandby = 0;  

  sensorIn.settings.filter = 0;
  sensorOut.settings.filter = 0;  

  sensorIn.settings.tempOverSample = 1;
  sensorOut.settings.tempOverSample = 1;
  
  //pressOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
    sensorIn.settings.pressOverSample = 1;
    sensorOut.settings.pressOverSample = 1; 
  //humidOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  sensorIn.settings.humidOverSample = 1;
  sensorOut.settings.humidOverSample = 1;

  // SPI.setFrequency(400000);

  // I2Cthree.begin(I2C_BMP2_SDA,I2C_BMP2_SCL,200000);
  I2Ctwo.begin(I2C_BME1_SDA,I2C_BME1_SCL,200000);

  delay(1000);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  Serial.println(sensorIn.beginI2C(I2Ctwo), HEX);
  delay(100);
  Serial.println(sensorOut.beginI2C(I2Ctwo), HEX);
  // Serial.println(sensorOut.begin(), HEX);
  // Serial.println(sensorIn.begin(), HEX);
}


//quick dp approximation, http://en.wikipedia.org/wiki/Dew_point
float dewpoint(float temperature, float humidity)
{
  float a = 17.271;
  float b = 237.7;
  float temp = (a * temperature) / (b + temperature) + log((double) humidity/100);
  double Td = (b * temp) / (a - temp);
  return Td;
}

uint32_t taskSensors()
{
  sensors_.tempIn = sensorIn.readTempC();
  sensors_.tempOut= sensorOut.readTempC();
  sensors_.rhIn =  sensorIn.readFloatHumidity();
  sensors_.rhOut = sensorOut.readFloatHumidity();
  sensors_.dpIn = dewpoint(sensors_.tempIn, sensors_.rhIn);
  sensors_.dpOut = dewpoint(sensors_.tempOut, sensors_.rhOut);
  sensors_.pressureIn = sensorIn.readFloatPressure();
  sensors_.pressureOut = sensorOut.readFloatPressure();
  sensors_.lastupdate = millis();
  Serial.println("Tin: "+String(sensors_.tempIn));
  Serial.println("Tout: "+String(sensors_.tempOut));
  Serial.println("DPin: "+String(sensors_.dpIn));
  Serial.println("DPout: "+String(sensors_.dpOut));
  Serial.println("HPaIn: "+String(sensors_.pressureIn));
  Serial.println("HPaOut: "+String(sensors_.pressureOut));
  return 15000;
}