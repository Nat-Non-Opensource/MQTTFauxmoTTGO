#include <Arduino.h>
#include <MqttConnector.h>
#include <Wire.h>

extern MqttConnector *mqtt;
extern fauxmoESP fauxmo;

extern String MQTT_CLIENT_ID;
extern String MQTT_PREFIX;

extern char myName[];
extern bool lcdState;

extern const char *ID_TFT;

void register_receive_hooks()
{
  mqtt->on_subscribe([&](MQTT::Subscribe *sub) -> void {
    Serial.printf("myName = %s \r\n", myName);
    sub->add_topic(MQTT_PREFIX + myName + "/$/+");
    sub->add_topic(MQTT_PREFIX + MQTT_CLIENT_ID + "/$/+");
  });

  mqtt->on_before_message_arrived_once([&](void) {});

  mqtt->on_message([&](const MQTT::Publish &pub) {});

  mqtt->on_after_message_arrived([&](String topic, String cmd, String payload) {
    Serial.printf("topic: %s\r\n", topic.c_str());
    Serial.printf("cmd: %s\r\n", cmd.c_str());
    Serial.printf("payload: %s\r\n", payload.c_str());
    if (cmd == "$/command")
    {
      if (payload == "ON")
      {
        // lcdState == true;
        fauxmo.setState(ID_TFT, true, 255);
      }
      else if (payload == "OFF")
      {
        // lcdState == false;
        fauxmo.setState(ID_TFT, false, 0);
      }
    }
    else if (cmd == "$/reboot")
    {
      ESP.restart();
    }
    else
    {
    }
  });
}
