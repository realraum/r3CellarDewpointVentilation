#include <Arduino.h>
// #include <SPI.h>
#include <SSD1306Wire.h>
#include <stdint.h>
#include <WiFi.h>
#include <MQTT.h>
#include "mqtt.h"
#include "vent.h"
#include "sensors.h"
#include "config.h"
#include "display.h"


void setup()
{                
  Serial.begin(115200);
  Serial.println();

  initDisplay();
  initSensors();
  initRelais();
  initWifiMqtt();
}



uint32_t taskPublishMqtt()
{
  publishMQTTData(getSensorData());
  return 45000;
}


uint32_t min2(uint32_t a, uint32_t b)
{
  uint32_t v=a;
  if (v > b) {v=b;};
  return v;
}

uint32_t min3(uint32_t a, uint32_t b, uint32_t c)
{
  uint32_t v=a;
  if (v > b) {v=b;};
  if (v > c) {v=c;};
  return v;
}


const int num_tasks_ = 5;
uint32_t (*tasks_[num_tasks_])() = {taskSensors,taskVentOrNot,taskDisplay,taskWifiMqtt,taskPublishMqtt};
uint32_t nexttime_[num_tasks_] = {0,0,0,0,0};


void loop()
{
  uint32_t now = millis();


  for (uint32_t t=0; t<num_tasks_; t++)
  {
    if (now >= nexttime_[t]) {
      nexttime_[t] = now + tasks_[t]();
    }
  }
}

