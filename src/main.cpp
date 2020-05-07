#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <ArduinoJson.h>
#include <MqttConnector.h>
#include "fauxmoESP.h"
#include "TFT_eSPI.h"

#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"
#include "_config.h"

// #define T4_V12
#define T4_V13
// #define T10_V14
// #define T10_V18
// #define T10_V20

#if defined(T10_V18)
#include "T10_V18.h"
#elif defined(T10_V14)
#include "T10_V14.h"
#elif defined(T4_V12)
#include "T4_V12.h"
#elif defined(T4_V13)
#include "T4_V13.h"
#elif defined(T10_V20)
#include "T10_V20.h"
#else
#error "Please select board version."
#endif

TFT_eSPI tft = TFT_eSPI();
MqttConnector *mqtt;
fauxmoESP fauxmo;

#define WIFI_SSID "CMMC_Sinet_2.4G"
#define WIFI_PASS "zxc12345"

#define SERIAL_BAUDRATE 115200

char myName[40];

const char *ID_TFT = "hybrid lamp";

void serialSetup()
{
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  Serial.println();
}

void wifiSetup()
{
  WiFi.disconnect();
  delay(20);

  WiFi.mode(WIFI_STA);
  delay(50);

  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  Serial.printf("[WIFI] CONNECTED | STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void lcdSetup()
{
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_RED);

  if (TFT_BL > 0)
  {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  }
}

void setup()
{
  serialSetup();
  wifiSetup();
  lcdSetup();
  fauxmo.createServer(true);
  fauxmo.setPort(80);

  fauxmo.enable(true);
  fauxmo.addDevice(ID_TFT);

  fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state, unsigned char value) {
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

    if (strcmp(device_name, ID_TFT) == 0)
    {
      tft.fillScreen(state ? TFT_GREEN : TFT_RED);
    }
  });
  init_mqtt();
}

void loop()
{
  fauxmo.handle();

  mqtt->loop();
}
