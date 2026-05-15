#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// #define LOCAL_BROKER

extern float glob_temperature;
extern float glob_humidity;
extern bool led_state;
extern bool fan_state;
extern int fanMode;
extern String WIFI_SSID;
extern String WIFI_PASS;
extern String CORE_IOT_TOKEN;
extern String CORE_IOT_SERVER;
extern String CORE_IOT_PORT;

extern boolean isWifiConnected;
extern SemaphoreHandle_t xBinarySemaphoreInternet;
extern SemaphoreHandle_t xLedMutex;
extern SemaphoreHandle_t xNeoPixelMutex;

#define DHT_PIN 4
#define DHT_TYPE DHT22

#endif