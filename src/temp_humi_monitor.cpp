#include "temp_humi_monitor.h"
DHT dht(DHT_PIN, DHT_TYPE);
// Khai báo biến toàn cục
LiquidCrystal_I2C lcd(33, 16, 2); // Lưu ý: Kiểm tra kỹ địa chỉ 33 (0x21)

// Hàm setup riêng cho phần này (được gọi 1 lần từ main.cpp)
void setup_monitoring_system() {    
    // 2. Khởi tạo phần cứng
    dht = DHT(DHT_PIN, DHT_TYPE);
    dht.begin();
    
    lcd.begin();      
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("System Booting..");
}

void temp_humi_monitor(void *pvParameters){

    while (1){
        // --- ĐỌC CẢM BIẾN ---
        dht.read();
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        if (isnan(temperature) || isnan(humidity)) {
            Serial.println("Failed to read DHT!");
            // Nếu lỗi, gán đại giá trị 0
            temperature = 0; 
            humidity = 0; 
        } else {
            // Cập nhật biến toàn cục
            glob_temperature = temperature;
            glob_humidity = humidity;
        }
        
        if (!isnan(glob_temperature) && !isnan(glob_humidity)) {
                // 2. Tạo chuỗi JSON
                // Format: {"temp": 28.5, "humi": 65.0}
            String json = "{\"temp\":" + String(glob_temperature) + 
              ",\"humi\":" + String(glob_humidity) + 
              ",\"server\":\"" + String(CORE_IOT_SERVER) + ":" + String(CORE_IOT_PORT) + "\"" +
              ",\"token\":\"" + String(CORE_IOT_TOKEN) + "\"}";

                // 3. Gửi lên Web Server
                Webserver_sendata(json); 
                Serial.println(json); // Debug
            }
        // --- IN RA SERIAL ---
        //Serial.printf("Hum: %.1f%%  Temp: %.1fC\n", humidity, temperature);
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
void Webserver_send_sensor(float temp, float humi);
