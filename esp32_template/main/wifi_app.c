/*
 * wifi_app.c
 *
 *  Created on: Nov 19, 2023
 *      Author: Phong Nguyen
 */
#include "esp_log.h"
#include "esp_err.h"

#include "esp_event.h"

#include "esp_wifi.h"
#include "lwip/netdb.h"

#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"

#include "tasks_common.h"
#include "rgb_led.h"
#include "wifi_app.h"

// Tag used for ESP Serial Console Message
static const char TAG[] = "___WIFI_APP___";

// Queue handle
static QueueSetHandle_t wifi_app_queue_handle;

// netif objects for the STATION / ACCESS POINT
esp_netif_t *esp_netif_sta = NULL;
esp_netif_t *esp_netif_ap = NULL;

BaseType_t wifi_app_send_message(wifi_app_message_t msgID) {
	ESP_LOGI(TAG,"SEND MESSAGE !");
	wifi_app_queue_message_t msg;
	msg.msgId = msgID;
	return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

/**
 * Configures the WiFi Access Point settings and assigns the static IP to the SoftAP.
 */
static void wifi_app_soft_ap_config(void) {
	// SoftAP - WiFi access point configuration
	wifi_config_t ap_config =
	{
		.ap = {
				.ssid = WIFI_AP_SSID,
				.ssid_len = strlen(WIFI_AP_SSID),
				.password = WIFI_AP_PASSWORD,
				.channel = WIFI_AP_CHANNEL,
				.ssid_hidden = WIFI_AP_SSID_HIDDEN,
				.authmode = WIFI_AUTH_WPA2_PSK,
				.max_connection = WIFI_AP_MAX_CONNECTIONS,
				.beacon_interval = WIFI_AP_BEACON_INTERVAL,
		},
	};

	// Configure DHCP for the application
	esp_netif_ip_info_t ap_ip_info;

	// Clear
	memset(&ap_ip_info, 0x00, sizeof(ap_ip_info));

	esp_netif_dhcps_stop(esp_netif_ap);
	inet_pton(AF_INET, WIFI_AP_IP, &ap_ip_info.ip); ///> Assign access point's static IP, GW, and netmask
	inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
	inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);

	//Set IP -  Statically configure the network interface
	ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &ap_ip_info));
	// Start the AP DHCP server (for connecting stations e.g. your mobile device)
	ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap));

	// Setting the mode as Access Point / Station Mode
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	// Set configuration
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
	// Default bandwidth 20 MHz
	ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH));
	//Power save set to "NONE"
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE));
}

/**
 * Initializes the TCP stack and default WIFI Configuration
 */
static void wifi_app_default_wifi_init() {
	// Initialize the TCP stack
	ESP_ERROR_CHECK(esp_netif_init());

	// Default WIFI config
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	esp_netif_sta = esp_netif_create_default_wifi_sta();
	esp_netif_ap = esp_netif_create_default_wifi_ap();
}

/**
 * WiFi application event handler
 * @param arg data, aside from event data, that is passed to the handler when it is called
 * @param event_base the base id of the event to register the handler for
 * @param event_id the id fo the event to register the handler for
 * @param event_data event data
 */
static void wifi_app_event_handler(void *event_handler_arg,
		esp_event_base_t event_base, int32_t event_id, void *event_data) {
	ESP_LOGI(TAG,"Received the EVENT !");
	if (event_base == WIFI_EVENT) {
		// case of WIFI events
		switch (event_id) {
		case WIFI_EVENT_AP_START:
			ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
			break;

		case WIFI_EVENT_AP_STOP:
			ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
			break;

		case WIFI_EVENT_AP_STACONNECTED:
			ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
			break;

		case WIFI_EVENT_AP_STADISCONNECTED:
			ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
			break;

		case WIFI_EVENT_STA_START:
			ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
			break;

		case WIFI_EVENT_STA_CONNECTED:
			ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
			break;

		case WIFI_EVENT_STA_DISCONNECTED:
			ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
			break;
		}
	} else if (event_base == IP_EVENT) {
		// case of IP event
		switch (event_id) {
		case IP_EVENT_STA_GOT_IP:
			ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
			break;
		}
	}
}

/**
 * Initialize the WIFI application event handler for WIFI and IP events
 */
static void wifi_app_event_handler_init() {
	// Event loop for the connection
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Event handler for the connection
	esp_event_handler_instance_t event_instance_wifi;
	esp_event_handler_instance_t event_instance_ip;

	// Register the events to WIFI event instance
	ESP_ERROR_CHECK(
			esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &event_instance_wifi));

	// Register the events to IP event instance
	ESP_ERROR_CHECK(
			esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &event_instance_ip));

}

/**void *pvParameters
 * Main task for WIFI application
 */
static void wifi_app_task(void *pvParameters) {
	//Store a message from Queue message
	wifi_app_queue_message_t msg;

	// Initialize the event handler
	wifi_app_event_handler_init();

	// Initialize the TCP/IP stack and WIFI config
	wifi_app_default_wifi_init();

	// SoftAP con-fig
	wifi_app_soft_ap_config();

	// Start WIFI
	ESP_ERROR_CHECK(esp_wifi_start());

	// Send test to event message
	wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SERVER);

	// Process when have the message
	for (;;) {
		if (xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY)) {
			// Case of receive the message from queue -> store into msg
			ESP_LOGI(TAG,"Received the MESSAGE !");
			switch (msg.msgId) {
			case WIFI_APP_MSG_START_HTTP_SERVER:
				ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");
				rgb_led_http_server_started();
				break;
			default:
				break;
			}
		}
	}
}

/**
 * Entry start
 */
void wifi_app_start() {
	ESP_LOGI(TAG, "STARTING WIFI APPLICATION");

	// RGB led display the color to indicate
	rgb_led_wifi_app_started();

	// Display default WIFI logging messages
	esp_log_level_set("WIFI", ESP_LOG_NONE);

	// Create Message Queue
	wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_message_t));

	// Start the WIFI Application task
	xTaskCreatePinnedToCore(&wifi_app_task, "WIFI_APP_TASK",
	WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL,
	WIFI_APP_TASK_CORE);
}
