#include "neo_blinky.h"
#include "global.h"

void neo_blinky(void *pvParameters){

    // Khởi tạo đối tượng NeoPixel
    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.setBrightness(50);
    strip.show();

    while(1) {                  
        uint32_t color = strip.Color(0, 0, 0); // Mặc định tắt
        if (glob_humidity < 40) {
            color = strip.Color(255, 0, 0);       // Đỏ (Rất khô)
        } else if (glob_humidity < 50){
            color = strip.Color(255, 165, 0);     // Cam
        } else if (glob_humidity < 60){
            color = strip.Color(255, 255, 0);     // Vàng
        } else if (glob_humidity < 70){
            color = strip.Color(0, 128, 0);       // Xanh lá (Lý tưởng)
        } else if (glob_humidity < 80){
            color = strip.Color(0, 0, 255);       // Xanh dương
        } else if (glob_humidity < 90){
            color = strip.Color(75, 0, 130);      // Chàm
        } else {
            color = strip.Color(128, 0, 128);     // Tím (Rất ẩm)
        }

        for(int i=0; i<LED_COUNT; i++) {
            strip.setPixelColor(i, color);
        }
        strip.show(); 
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}