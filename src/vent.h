#ifndef VENT__H
#define VENT__h


void initRelais(void);
uint32_t taskVentOrNot();
void manuallyRunVentForS(uint32_t s);
bool getVentState(void);
bool getManualOverrideState(void);


#endif