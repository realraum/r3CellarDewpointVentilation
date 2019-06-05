#include <Arduino.h>
#include "config.h"
#include "sensors.h"

void initRelais(void)
{
  pinMode(RELAY1,OUTPUT);
  pinMode(RELAY2,OUTPUT);
    //test r1
  digitalWrite(RELAY1,RELAY_ON);
  digitalWrite(RELAY2,RELAY_OFF);
  delay(150);
  //test r2
  digitalWrite(RELAY1,RELAY_OFF);
  digitalWrite(RELAY2,RELAY_ON);
  delay(150);
  //off
  digitalWrite(RELAY1,RELAY_OFF);
  digitalWrite(RELAY2,RELAY_OFF);
}

//Globals. We use longs because we are counting seconds, 16bit would overrun
uint32_t venting_ventingTime_s_=0;
uint32_t venting_notVentingTime_s_=0;
uint32_t venting_last_change_s_=0;
bool venting_onoff_=false;
uint32_t manual_venting_time_until_s_=0;



uint32_t taskVentOrNot()
{
  uint32_t now_s = millis()/1000;

  if (now_s > manual_venting_time_until_s_)
  {
    //reset manual override
    manual_venting_time_until_s_ = 0;
  }

  //Decide whether to vent
  if ( ((getSensorData()->dpOut+VENTTHRESHOLD)<getSensorData()->dpIn) and (getSensorData()->tempOut>15) ){
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

  if (manual_venting_time_until_s_ > 0)
  {
    //manual venting
    venting_onoff_ = true;
  }

  digitalWrite(RELAY2,(venting_onoff_)?RELAY_ON:RELAY_OFF);
  digitalWrite(RELAY1,(venting_onoff_)?RELAY_ON:RELAY_OFF);
  return 60000;
}

void manuallyRunVentForS(uint32_t s)
{
  if (s > MAX_MANUAL_VENT_TIME_S)
  {
    s = MAX_MANUAL_VENT_TIME_S;
  }
  if (s > 0)
  {
    manual_venting_time_until_s_ = millis()/1000 + s;
    digitalWrite(RELAY2,RELAY_ON);
    digitalWrite(RELAY1,RELAY_ON);
  } else {
    digitalWrite(RELAY2,RELAY_OFF);
    digitalWrite(RELAY1,RELAY_OFF);
  }
}