#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "rgb_led.h"
#include "esp_event.h"

void app_main(void) {
	while (true) {
		rgb_led_wifi_app_started();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		rgb_led_http_app_started();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		rgb_led_wifi_connected();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
