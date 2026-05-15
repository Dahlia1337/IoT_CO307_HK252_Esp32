#include "task_button.h"

void task_monitor_button_led(void *pvParameters)
{
    pinMode(LED_PIN, OUTPUT);
    while (1) {
        // Check if button is pressed (active-low)
        if (xSemaphoreTake(xBinarySemaphoreInternet, (TickType_t)10) == pdTRUE) {
            digitalWrite(LED_PIN, ledState);
            xSemaphoreGive(xBinarySemaphoreInternet); 
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_monitor_button_fan(void *pvParameters) {
    // Cấu hình chân quạt
    pinMode(GPIO_NUM_6, OUTPUT);
    analogWrite(GPIO_NUM_6, 0); 

    while (1) {
        int speed = 0;
        switch (fanMode) {
            case 0: 
                speed = 0;
                break;
            case 1: 
                speed = 100; 
                break;

            case 2: 
                speed = 255; 
                break;

            case 3: 
                if (result > 0.7) {
                    speed = 255; 
                } else if (result > 0.6) {
                    speed = 150; 
                } else {
                    speed = 0; 
                }
                break;
        }
        analogWrite(GPIO_NUM_6, speed);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}