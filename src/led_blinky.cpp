#include "led_blinky.h"
#include "task_core_iot.h"

void led_control(int state)
{
  pinMode(LED1_GPIO, OUTPUT);
  if (state)
    digitalWrite(LED1_GPIO, LOW);
  else
    digitalWrite(LED1_GPIO, HIGH);
}

void led_blinky(void *pvParameters)
{

  pinMode(LED2_GPIO, OUTPUT);

  while (1)
  {
    digitalWrite(LED2_GPIO, HIGH); // turn the LED ON
    vTaskDelay(1000);
    digitalWrite(LED2_GPIO, LOW); // turn the LED OFF
    vTaskDelay(1000);
  }
}