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

void initButton()
{
  pinMode(BUTTON_PIN,(BUTTON_PRESSED==LOW?INPUT_PULLUP:INPUT_PULLDOWN));
  digitalWrite(BUTTON_PIN, (BUTTON_PRESSED==HIGH?LOW:HIGH));
}


void setup()
{                
  Serial.begin(115200);
  Serial.println();

  initDisplay();
  initSensors();
  initRelais();
  initWifiMqtt();
  initButton();
}



uint32_t taskPublishMqtt()
{
  publishMQTTData(getSensorData(),getVentState(),getManualOverrideState());
  return 45000;
}

uint32_t taskButton()
{
  static uint16_t btn_count_=0;
  if (digitalRead(BUTTON_PIN) == BUTTON_PRESSED)
  {
    if (btn_count_ < BUTTON_DEBOUNCE+1)
    {
      btn_count_++;
    }
  } else {
    btn_count_=0;
  }

  if (btn_count_ == BUTTON_DEBOUNCE)
  {
    manuallyRunVentForS(900); //15min
  }
  return 5;
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


const int num_tasks_ = 6;
uint32_t (*tasks_[num_tasks_])() = {taskSensors,taskVentOrNot,taskButton,taskDisplay,taskWifiMqtt,taskPublishMqtt};
uint32_t nexttime_[num_tasks_] = {0,0,0,0,0,0};


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

