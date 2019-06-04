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
  Serial.print("WIFI Connecting to:");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_pass);
  delay(1000);
  mqttc.begin(wifi_mqttbroker, net);
  // mqttc.onMessage(mqttMessageReceived);
}

uint32_t taskWifiMqtt()
{
  static MQTTSTATE_T mqtt_state = WIFI_DISCONNECTED;
  uint32_t now_s = millis()/1000;

  if (WiFi.status() != WL_CONNECTED)
  {
    if (mqtt_state != WIFI_DISCONNECTED)
    {
      Serial.println("WIFI disconnected");
    }
    mqtt_state = WIFI_DISCONNECTED;
  } else if (mqtt_state == WIFI_DISCONNECTED)
  {
    //if newly connected, set state to newly connected WIFI_CONNECTED_MQTT_DISCONNECTED
    mqtt_state = WIFI_CONNECTED_MQTT_DISCONNECTED;
    Serial.println("WIFI connected");
    //continue
  } else if (!mqttc.connected())
  {
    Serial.println("MQTT disconnected");
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
        Serial.println("MQTT connected");
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
  Serial.println("MQTT publish");
  // mqttc.publish(topic, msg);
  mqttc.publish(
    String("realraum/")+wifi_mqttclientid+"/barometer",
    String("{\"HPa\": ")+String(pressureIn)+",\"Location\": \""+wifi_mqttlocation_inside+"\"}"
    );
  mqttc.publish(
    String("realraum/")+wifi_mqttclientid+"/temperature",
    String("{\"Value\": ")+String(tempIn)+",\"Location\": \""+wifi_mqttlocation_inside+"\"}"
    );
  mqttc.publish(
    String("realraum/")+wifi_mqttclientid+"/temperature",
    String("{\"Value\": ")+String(tempOut)+",\"Location\": \""+wifi_mqttlocation_outside+"\"}"
    );
}