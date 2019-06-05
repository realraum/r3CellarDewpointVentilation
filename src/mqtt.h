#ifndef MQTT__H
#define MQTT__H

#include "sensors.h"

void initWifiMqtt();
uint32_t taskWifiMqtt();
void publishMQTTData(GlobalSensorData const *sd);

#endif