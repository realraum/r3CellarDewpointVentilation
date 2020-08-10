#ifndef MQTT__H
#define MQTT__H

#include "sensors.h"

void initWifiMqtt();
uint32_t taskWifiMqtt();
void publishMQTTData(GlobalSensorData const *sd, bool vent_on, bool due_to_buttonpress);

#endif