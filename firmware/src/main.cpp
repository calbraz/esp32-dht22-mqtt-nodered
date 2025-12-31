#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

static constexpr uint8_t DHT_PIN = 15;

static constexpr const char *WIFI_SSID = "Wokwi-GUEST";
static constexpr const char *WIFI_PASS = "";

// Wokwi → local host broker alias
static constexpr const char *MQTT_HOST = "host.wokwi.internal";
static constexpr uint16_t MQTT_PORT = 1883;

static constexpr const char *TOPIC_TEMP = "/thinkiot/esp32-dht22-mqtt-nodered/temp";
static constexpr const char *TOPIC_HUM = "/thinkiot/esp32-dht22-mqtt-nodered/hum";

static constexpr uint32_t PUBLISH_INTERVAL_MS = 2000;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
DHTesp dht;

uint32_t lastPublish = 0;
uint32_t lastReconnectAttempt = 0;

static void connectWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.printf("WiFi connecting to '%s'", WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        Serial.print('.');
    }
    Serial.printf("\nWiFi connected. IP=%s\n", WiFi.localIP().toString().c_str());
}

static bool connectMQTT()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char clientId[32];
    snprintf(clientId, sizeof(clientId), "esp32-%02X%02X%02X", mac[3], mac[4], mac[5]);

    Serial.printf("MQTT connect %s:%u ... ", MQTT_HOST, MQTT_PORT);
    bool ok = mqtt.connect(clientId);
    Serial.println(ok ? "OK" : "FAIL");
    if (!ok)
        Serial.printf("MQTT state=%d\n", mqtt.state());
    return ok;
}

static void publishReading()
{
    TempAndHumidity data = dht.getTempAndHumidity();
    if (isnan(data.temperature) || isnan(data.humidity))
    {
        Serial.println("DHT read failed (NaN).");
        return;
    }

    char t[16], h[16];
    snprintf(t, sizeof(t), "%.2f", data.temperature);
    snprintf(h, sizeof(h), "%.1f", data.humidity);

    mqtt.publish(TOPIC_TEMP, t);
    mqtt.publish(TOPIC_HUM, h);

    Serial.printf("Published: T=%s C, H=%s %%\n", t, h);
}

void setup()
{
    Serial.begin(115200);
    delay(200);

    connectWiFi();

    mqtt.setServer(MQTT_HOST, MQTT_PORT);

    dht.setup(DHT_PIN, DHTesp::DHT22);

    connectMQTT();
}

void loop()
{
    if (!mqtt.connected())
    {
        uint32_t now = millis();
        if (now - lastReconnectAttempt > 3000)
        {
            lastReconnectAttempt = now;
            connectMQTT();
        }
        delay(10);
        return;
    }

    mqtt.loop();

    uint32_t now = millis();
    if (now - lastPublish > PUBLISH_INTERVAL_MS)
    {
        lastPublish = now;
        publishReading();
    }
}
