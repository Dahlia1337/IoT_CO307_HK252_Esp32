#include <task_handler.h>
#include "component_control.h"
void handleWebSocketMessage(String message)
{
    Serial.println(message);
    StaticJsonDocument<512> doc;

    DeserializationError error = deserializeJson(doc, message);
    if (error)
    {
        Serial.println("❌ Lỗi parse JSON!");
        return;
    }
    JsonObject value = doc["value"];
    if (doc["page"] == "device")
    {
        int mode = doc["value"]["mode"];
        String status = doc["value"]["status"];

        Serial.printf("Web Control -> MODE: %d | Status: %s\n", mode, status.c_str());

        // Chỉ xử lý nếu trạng thái là ON (người dùng chọn chế độ đó)
        if (status) {
            switch (mode) {
                case 0: 
                    fanMode = 0;
                    Serial.println("=> MODE: Fan OFF");
                    fan_control(0);
                    break;
                case 1:  
                    fanMode = 1;
                    Serial.println("=> MODE: Fan Level 1");
                    fan_control(1);
                    break;
                case 2: 
                    fanMode = 2;
                    Serial.println("=> MODE: Fan Level 2");
                    fan_control(2);
                    break;
                case 3: 
                    fanMode = 3;
                    Serial.println("=> MODE: Fan Auto");
                    fan_control(3);
                    break;
                default:
                    Serial.println("=> Unknown MODE command");
                    break;
            }
        }
        String fanNames[] = {"OFF", "Level 1", "Level 2", "Auto"};
        String reply = "{\"fan_mode\":" + String(fanMode) +
                ",\"fan_name\":\"" + fanNames[fanMode] + "\"}";
        ws.textAll(reply);
    }
    else if (doc["page"] == "setting")
    {

        String WIFI_SSID = doc["value"]["ssid"].as<String>();
        String WIFI_PASS = doc["value"]["password"].as<String>();

#ifndef LOCAL_BROKER
        String CORE_IOT_TOKEN = doc["value"]["token"].as<String>();
        String CORE_IOT_SERVER = doc["value"]["server"].as<String>();
        String CORE_IOT_PORT = doc["value"]["port"].as<String>();
#endif

        Serial.println("📥 Nhận cấu hình từ WebSocket:");
        Serial.println("SSID: " + WIFI_SSID);
        Serial.println("PASS: " + WIFI_PASS);
        Serial.println("TOKEN: " + CORE_IOT_TOKEN);
        Serial.println("SERVER: " + CORE_IOT_SERVER);
        Serial.println("PORT: " + CORE_IOT_PORT);

        // 👉 Gọi hàm lưu cấu hình
        Save_info_File(WIFI_SSID, WIFI_PASS, CORE_IOT_TOKEN, CORE_IOT_SERVER, CORE_IOT_PORT);

        // Phản hồi lại client (tùy chọn)
        String msg = "{\"status\":\"ok\",\"page\":\"setting_saved\"}";
        ws.textAll(msg);
    }
}