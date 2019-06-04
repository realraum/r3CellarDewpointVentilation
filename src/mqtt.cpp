#include <WiFi.h>
#include <MQTT.h>
#include "wifipwd.h"
#include "wifipwd.cpp"

WiFiClient net;
MQTTClient mqttc;

typedef enum {WIFI_DISCONNECTED, WIFI_CONNECTED_MQTT_DISCONNECTED, MQTT_CONNECTED} MQTTSTATE_T;

// void mqttMessageReceived(String &topic, String &payload) {
//   Serial.println("incoming: " + topic + " - " + payload);
// }

void initWifiMqtt()
{
  WiFi.begin(wifi_ssid, wifi_pass);
  mqttc.begin(wifi_mqttbroker, net);
  // mqttc.onMessage(mqttMessageReceived);
}

uint32_t taskWifiMqtt()
{
  static MQTTSTATE_T mqtt_state = WIFI_DISCONNECTED;
  uint32_t now_s = millis()/1000;

  if (WiFi.status() != WL_CONNECTED)
  {
    mqtt_state = WIFI_DISCONNECTED;
  } else if (mqtt_state == WIFI_DISCONNECTED)
  {
    //if newly connected, set state to newly connected WIFI_CONNECTED_MQTT_DISCONNECTED
    mqtt_state = WIFI_CONNECTED_MQTT_DISCONNECTED;
    //continue
  } else if (!mqttc.connected())
  {
    mqtt_state = WIFI_CONNECTED_MQTT_DISCONNECTED;
  }


  switch (mqtt_state)
  {
    case WIFI_DISCONNECTED:
      return now_s+1000;
      break;
    case WIFI_CONNECTED_MQTT_DISCONNECTED:
      if (mqttc.connect(wifi_mqttclientid, wifi_mqttuser, wifi_mqttpass))
      {
        // mqttc.subscribe("realraum/");
        // mqttc.subscribe("realraum/");
        mqtt_state = MQTT_CONNECTED;
        return now_s+100;
      }
      //else
      return now_s+1000;
      break;
    case MQTT_CONNECTED:
      mqttc.loop();
      return now_s+250;
      break;
    default:
      mqtt_state = WIFI_CONNECTED_MQTT_DISCONNECTED;
      return now_s+1000;
      break;
  }
}

void publishMQTTData(float tempIn,float tempOut,float rhIn,float rhOut,float dpIn,float dpOut,float pressureIn,float pressureOut)
{
  if (!mqttc.connected())
  {
    return;
  }
  // mqttc.publish(topic, msg);
}