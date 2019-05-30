#include <SPI.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include <stdint.h>
#include "SparkFunBME280.h"


/*
Notes:

* auf I2C kein Device gefunden wenn auf I2Ctwo gesucht wird.
* Mit BME280 an VDD+GND angehängt, kein Flashen möglich? Saugt BME280 soviel?
* mit BME280 an SPI angehängt -> Flash Read Error ???


*/


#define I2C_DISPLAY_SDA 5
#define I2C_DISPLAY_SCL 4

#define I2C_BME1_SDA 13
#define I2C_BME1_SCL 16
#define I2C_BMP2_SDA 15
#define I2C_BMP2_SCL 14
TwoWire I2Ctwo = TwoWire(1);
TwoWire I2Cthree = TwoWire(2);

#include "display.h"

//Relay control pins
#define RELAY1 8
#define RELAY2 9

//Set the frequency of updating the relay here, in minutes. The larger the value, the less likely you will have rapid on/off cycling
#define RELAYINTERVAL 1

//Set the ventilation threshold of the DEWPOINT in degrees celcius. I recccomend it being greater than 1, as that seems to be the margin of error for these sensors
#define VENTTHRESHOLD 1.5

//Set cutoff temp for ventilation, celcius
#define TEMPCUTOFF 15



//Define sensor objects
BME280 sensorIn;
BME280 sensorOut;

void initSensors(void)
{

  //Initialize Sensors
  // sensorIn.settings.commInterface = SPI_MODE;
  // sensorIn.settings.chipSelectPin = 24;
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

  delay(1000);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  Serial.println(sensorIn.beginI2C(I2Cthree), HEX);
  Serial.println(sensorOut.beginI2C(I2Ctwo), HEX);
  // Serial.println(sensorOut.begin(), HEX);
  // Serial.println(sensorIn.begin(), HEX);
}

void initRelais(void)
{
  //Initialize Relays, set to OFF (which is HIGH, arduino set to LOW at boot)
  pinMode(RELAY1,OUTPUT);
  pinMode(RELAY2,OUTPUT);
  digitalWrite(RELAY1,HIGH);
  digitalWrite(RELAY2,HIGH);
}

void setup()
{                
  Serial.begin(115200);
  Serial.println();

  I2Cthree.begin(I2C_BMP2_SDA,I2C_BMP2_SCL,200000);
  I2Ctwo.begin(I2C_BME1_SDA,I2C_BME1_SCL,200000);

  initDisplay();
  initSensors();
}


float sensors_tempIn_=0.0;
float sensors_tempOut_=0.0;
float sensors_rhIn_=0.0;
float sensors_rhOut_=0.0;
float sensors_dpIn_=0.0;
float sensors_dpOut_=0.0;
uint32_t sensors_lastupdate_=0;


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
  sensors_tempIn_ = sensorIn.readTempC();
  sensors_tempOut_= sensorOut.readTempC();
  sensors_rhIn_ =  sensorIn.readFloatHumidity();
  sensors_rhOut_ = sensorOut.readFloatHumidity();
  sensors_dpIn_ = dewpoint(sensors_tempIn_,sensors_rhIn_);
  sensors_dpOut_ = dewpoint(sensors_tempOut_,sensors_rhOut_);
  sensors_lastupdate_ = millis();
  Serial.println("Tin: "+String(sensors_tempIn_));
  Serial.println("Tout: "+String(sensors_tempOut_));
  Serial.println("DPin: "+String(sensors_dpIn_));
  Serial.println("DPout: "+String(sensors_dpOut_));
  return 15000;
}

//Globals. We use longs because we are counting seconds, 16bit would overrun
uint32_t venting_ventingTime_s_=0;
uint32_t venting_notVentingTime_s_=0;
uint32_t venting_last_change_s_=0;
bool venting_onoff_=false;

uint32_t taskVentOrNot()
{
  uint32_t now_s = millis()/1000;
  //Decide whether to vent
  if ( ((sensors_dpOut_+VENTTHRESHOLD)<sensors_dpIn_) and (sensors_tempOut_>15) ){
      if (false == venting_onoff_)
      {
        venting_notVentingTime_s_ += now_s - venting_last_change_s_;
        venting_last_change_s_ = now_s;
      }
      venting_onoff_=true;
  }
  else
  {
      if (false == venting_onoff_)
      {
        venting_ventingTime_s_ += now_s - venting_last_change_s_;
        venting_last_change_s_ = now_s;
      }
      venting_onoff_=false;
  }

  digitalWrite(RELAY2,(venting_onoff_)?LOW:HIGH);      
  digitalWrite(RELAY1,(venting_onoff_)?LOW:HIGH);
  return 60000;
}

uint32_t min3(uint32_t a, uint32_t b, uint32_t c)
{
  uint32_t v=a;
  if (v > b) {v=b;};
  if (v > c) {v=c;};
  return v;
}

void loop()
{
  static uint32_t nt_sensors=0;
  static uint32_t nt_vent=0;
  static uint32_t nt_display=0;
  uint32_t dl_sensors=0;
  uint32_t dl_vent=0;
  uint32_t dl_display=0;

  uint32_t now = millis();

  if (now >= nt_sensors)
  {
    dl_sensors = taskSensors(); 
    nt_sensors = now + dl_sensors; 
  }
  if (now >= nt_vent)
  {
    dl_vent = taskVentOrNot();
    nt_vent = now + dl_vent;
  }
  if (now >= nt_display)
  {
    dl_display = taskDisplay();
    nt_display = now + dl_display;
  }
  delay(min3(dl_display,dl_sensors,dl_vent)+1);
}

