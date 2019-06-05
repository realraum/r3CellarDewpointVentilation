#ifndef SENSORS__H
#define SENSORS__H

typedef struct {
	float tempIn;
	float tempOut;
	float rhIn;
	float rhOut;
	float dpIn;
	float dpOut;
	float pressureIn;
	float pressureOut;
	uint32_t lastupdate;
} GlobalSensorData;

GlobalSensorData const *getSensorData();
void initSensors(void);
uint32_t taskSensors();

#endif