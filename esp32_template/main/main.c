<<<<<<< HEAD
/**
 * Application entry point
 */
#include "nvs_flash.h"

#include "wifi_app.h"
void app_main(void) {
    // Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(ret);
	// Start Wifi
	wifi_app_start();

//	while (true) {
//		rgb_led_wifi_app_started();
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//		rgb_led_http_app_started();
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//		rgb_led_wifi_connected();
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//	}
=======
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_event.h"
#include "rgb_led.h"
#include "wifi_app.h"
#include "nvs_flash.h"
#include "dht22.h"
void app_main(void) {
	// Initialize NVS
		esp_err_t ret = nvs_flash_init();
		if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
		{
			ESP_ERROR_CHECK(nvs_flash_erase());
			ret = nvs_flash_init();
		}
		ESP_ERROR_CHECK(ret);

		// Start Wifi
		wifi_app_start();

		// Start DHT
		DHT22_task_start();
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
}
