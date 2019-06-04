#ifndef MQTT__H
#define MQTT__H

void initWifiMqtt();
uint32_t taskWifiMqtt();
void publishMQTTData(float tempIn,float tempOut,float rhIn,float rhOut,float dpIn,float dpOut,float pressureIn,float pressureOut);

#endif