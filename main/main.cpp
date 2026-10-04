#include <cstdio>

#include "epd213.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "main";

extern "C" void app_main(void) {
    using epd::Color;

    static epd::Epd213 display;  // T5 V2.3.x pins by default
    ESP_ERROR_CHECK(display.begin());

    display.setRotation(epd::Rotation::Landscape);  // 250 x 122
    display.clear();

    // Static content
    display.drawRect(0, 0, display.width(), display.height());
    display.drawText(8, 8, "Hello, ESP-IDF!", 2);
    display.drawText(8, 30, "LILYGO T5 V2.3.2 2.13in e-paper", 1);
    display.drawLine(8, 44, display.width() - 9, 44);
    // display.drawLine(0, 0, display.width() - 1, display.height() - 1);
    display.drawText(8, 100, "pure ESP-IDF / C++", 1);

    ESP_LOGI(TAG, "full refresh");
    ESP_ERROR_CHECK(display.refreshFull());

    // Fast partial updates: a counter
    constexpr int kPartialsBeforeFull = 5;
    for (int i = 1; i <= kPartialsBeforeFull; ++i) {
        vTaskDelay(pdMS_TO_TICKS(2000));

        char buf[24];
        snprintf(buf, sizeof(buf), "Count: %d", i);
        display.fillRect(100, 60, 140, 24, Color::White);
        display.drawText(100, 64, buf, 2);

        ESP_LOGI(TAG, "partial refresh %d", i);
        ESP_ERROR_CHECK(display.refreshPartial());
    }

    // A full refresh clears accumulated ghosting
    display.fillRect(100, 60, 140, 24, Color::White);
    display.drawText(100, 64, "Done", 2);
    ESP_ERROR_CHECK(display.refreshFull());

    // Put the panel to sleep: an e-paper must not stay powered with a static image.
    display.sleep();
    ESP_LOGI(TAG, "panel asleep, finished");
}
