#ifndef __LED_BLINKY__
#define __LED_BLINKY__
#include <Arduino.h>
#include "global.h"

#define LED1_GPIO 43
#define LED2_GPIO 44
void led_control(int state);
void led_blinky(void *pvParameters);


#endif