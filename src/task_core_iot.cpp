#include "task_core_iot.h"

// ============================================================
// Cấu hình
// ============================================================
#define LED_PIN 48

constexpr uint32_t MAX_MESSAGE_SIZE      = 1024U;
constexpr uint32_t TELEMETRY_INTERVAL_MS = 10000U;

constexpr char LED_STATE_ATTR[] = "ledState";
constexpr char FAN_STATE_ATTR[] = "fanState";

double longitude = 106.80633605864662;
double latitude = 10.880018410410052;

// ============================================================
// Biến trạng thái
// ============================================================
volatile bool ledState = false;
volatile bool fanState = false;

// ============================================================
// ThingsBoard client
// ============================================================
WiFiClient         wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard        tb(mqttClient, MAX_MESSAGE_SIZE);

// ============================================================
// RPC Callbacks
// ============================================================

/// Dashboard gửi lệnh bật/tắt LED
RPC_Response setLedSwitchValue(const RPC_Data &data)
{
    ledState = data.as<bool>();
    led_control(ledState);
    tb.sendAttributeData(LED_STATE_ATTR, ledState);
    Serial.printf("[RPC] LED -> %s\n", ledState ? "ON" : "OFF");
    return RPC_Response(LED_STATE_ATTR, ledState);
}

/// Dashboard hỏi trạng thái LED
RPC_Response getLedState(const RPC_Data &data)
{
    return RPC_Response(LED_STATE_ATTR, ledState);
}

/// Dashboard gửi lệnh bật/tắt quạt
RPC_Response setFanSwitchValue(const RPC_Data &data)
{
    fanState = data.as<bool>();
    tb.sendAttributeData(FAN_STATE_ATTR, fanState);
    Serial.printf("[RPC] FAN -> %s\n", fanState ? "ON" : "OFF");
    return RPC_Response(FAN_STATE_ATTR, fanState);
}

/// Dashboard hỏi trạng thái quạt
RPC_Response getFanState(const RPC_Data &data)
{
    return RPC_Response(FAN_STATE_ATTR, fanState);
}

/// Tên callback PHẢI khớp với cấu hình "Get/Set value method" trên widget ThingsBoard
const std::array<RPC_Callback, 4U> rpcCallbacks = {{
    { "setLedSwitchValue", setLedSwitchValue },
    { "getLedSwitchValue", getLedState       },
    { "setFanSwitchValue", setFanSwitchValue },
    { "getFanSwitchValue", getFanState       },
}};

// ============================================================
// Shared Attributes
// ============================================================

void processSharedAttributes(const Shared_Attribute_Data &data)
{
    if (data.containsKey(LED_STATE_ATTR))
    {
        ledState = data[LED_STATE_ATTR].as<bool>();
        led_control(ledState);
        Serial.printf("[Attr] LED -> %s\n", ledState ? "ON" : "OFF");
    }
    if (data.containsKey(FAN_STATE_ATTR))
    {
        fanState = data[FAN_STATE_ATTR].as<bool>();
        Serial.printf("[Attr] FAN -> %s\n", fanState ? "ON" : "OFF");
    }
}

constexpr std::array<const char *, 2U> SHARED_ATTR_LIST = {
    LED_STATE_ATTR,
    FAN_STATE_ATTR,
};

const Shared_Attribute_Callback  sharedAttrCallback(&processSharedAttributes,
                                                     SHARED_ATTR_LIST.cbegin(),
                                                     SHARED_ATTR_LIST.cend());
const Attribute_Request_Callback attrRequestCallback(&processSharedAttributes,
                                                      SHARED_ATTR_LIST.cbegin(),
                                                      SHARED_ATTR_LIST.cend());

// ============================================================
// Kết nối / đăng ký lại sau khi mất kết nối
// ============================================================

static void reconnectAndSubscribe()
{
    if (!tb.connect(CORE_IOT_SERVER.c_str(), CORE_IOT_TOKEN.c_str(), CORE_IOT_PORT.toInt()))
    {
        Serial.println("[TB] Connect failed, retry later...");
        return;
    }
    Serial.println("[TB] Connected");

    // Gửi attributes hệ thống một lần
    tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());
    tb.sendAttributeData("localIP",    WiFi.localIP().toString().c_str());
    tb.sendAttributeData("ssid",       WiFi.SSID().c_str());
    tb.sendAttributeData("wifiChannel", (int32_t)WiFi.channel());
    tb.sendAttributeData("longitude", longitude);
    tb.sendAttributeData("latitude", latitude);

    // Đăng ký RPC
    if (!tb.RPC_Subscribe(rpcCallbacks.cbegin(), rpcCallbacks.cend()))
        Serial.println("[TB] RPC subscribe failed");

    // Đăng ký shared attributes
    if (!tb.Shared_Attributes_Subscribe(sharedAttrCallback))
        Serial.println("[TB] Shared attr subscribe failed");

    // Yêu cầu giá trị hiện tại của shared attributes từ server
    if (!tb.Shared_Attributes_Request(attrRequestCallback))
        Serial.println("[TB] Shared attr request failed");
}

// ============================================================
// FreeRTOS Task
// ============================================================

void coreiot_task(void *pvParameters)
{
    (void)pvParameters;

    // Chờ WiFi sẵn sàng (semaphore được give từ wifi_task)
    xSemaphoreTake(xBinarySemaphoreInternet, portMAX_DELAY);
    Serial.println("[TB] WiFi ready, starting CoreIOT task");

    uint32_t lastTelemetrySend = 0U;

    while (1)
    {
        if (!tb.connected())
            reconnectAndSubscribe();

        tb.loop();

        if (tb.connected() && (millis() - lastTelemetrySend >= TELEMETRY_INTERVAL_MS))
        {
            tb.sendTelemetryData("temperature", glob_temperature);
            tb.sendTelemetryData("humidity",    glob_humidity);
            tb.sendTelemetryData("led_status",  ledState);
            tb.sendTelemetryData("fan_status",  fanState);
            lastTelemetrySend = millis();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}