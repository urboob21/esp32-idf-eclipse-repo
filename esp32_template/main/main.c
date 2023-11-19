#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_event.h"
#include "rgb_led.h"

void app_main(void) {
	while (true) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
