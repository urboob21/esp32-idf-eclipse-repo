/*
<<<<<<< HEAD
 * rgb_led.c
 *
 *  Created on: Oct 28, 2023
 *      Author: Phong Nguyen
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#include "rgb_led.h"
#include "tasks_common.h"
#include "wifi_app.h"
#include "a_http_server.h"

// Tag used for ESP serial console messages
static const char TAG[] = "wifi_app";

// Queue handle used to manipulate the main queue of events
static QueueHandle_t wifi_app_queue_handle;

// netif objects for the station and access point
esp_netif_t *esp_netif_sta = NULL;
esp_netif_t *esp_netif_ap = NULL;

=======
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
#include "http_server.h"

// Tag used for ESP Serial Console Message
static const char TAG[] = "___WIFI_APP___";

// Used for returning the WiFi configuration
wifi_config_t *wifi_config = NULL;

// Used to track the number for retries when a connection attempt fails
static int g_retry_number;

// Queue handle
static QueueSetHandle_t wifi_app_queue_handle;

// netif objects for the STATION / ACCESS POINT
esp_netif_t *esp_netif_sta = NULL;
esp_netif_t *esp_netif_ap = NULL;

BaseType_t wifi_app_send_message(wifi_app_message_t msgID)
{
	ESP_LOGI(TAG, "SEND MESSAGE !");
	wifi_app_queue_message_t msg;
	msg.msgId = msgID;
	return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

/**
 * Connects the ESP32 to an external AP using the updated station configuration
 */
static void wifi_app_connect_sta(void)
{
	ESP_ERROR_CHECK(
		esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_app_get_wifi_config()));
	ESP_ERROR_CHECK(esp_wifi_connect());
}

/**
 * Configures the WiFi Access Point settings and assigns the static IP to the SoftAP.
 */
static void wifi_app_soft_ap_config(void)
{
	// SoftAP - WiFi access point configuration
	wifi_config_t ap_config = {
		.ap = {
			.ssid = WIFI_AP_SSID,
			.ssid_len =
				strlen(WIFI_AP_SSID),
			.password = WIFI_AP_PASSWORD,
			.channel =
				WIFI_AP_CHANNEL,
			.ssid_hidden = WIFI_AP_SSID_HIDDEN,
			.authmode =
				WIFI_AUTH_WPA2_PSK,
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

	// Set IP -  Statically configure the network interface
	ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &ap_ip_info));
	// Start the AP DHCP server (for connecting stations e.g. your mobile device)
	ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap));

	// Setting the mode as Access Point / Station Mode
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	// Set configuration
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
	// Default bandwidth 20 MHz
	ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH));
	// Power save set to "NONE"
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE));
}

/**
 * Initializes the TCP stack and default WIFI Configuration
 */
static void wifi_app_default_wifi_init()
{
	// Initialize the TCP stack
	ESP_ERROR_CHECK(esp_netif_init());

	// Default WIFI config
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	esp_netif_sta = esp_netif_create_default_wifi_sta();
	esp_netif_ap = esp_netif_create_default_wifi_ap();
}

>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
/**
 * WiFi application event handler
 * @param arg data, aside from event data, that is passed to the handler when it is called
 * @param event_base the base id of the event to register the handler for
 * @param event_id the id fo the event to register the handler for
 * @param event_data event data
 */
<<<<<<< HEAD
static void wifi_app_event_handler(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data) {
	if (event_base == WIFI_EVENT) {
		switch (event_id) {
=======
static void wifi_app_event_handler(void *event_handler_arg,
								   esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	ESP_LOGI(TAG, "Received the EVENT !");
	if (event_base == WIFI_EVENT)
	{
		// case of WIFI events
		switch (event_id)
		{
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
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
<<<<<<< HEAD
			break;
		}
	} else if (event_base == IP_EVENT) {
		switch (event_id) {
		case IP_EVENT_STA_GOT_IP:
			ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
=======

			wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = (wifi_event_sta_disconnected_t *)malloc(sizeof(wifi_event_sta_disconnected_t));
			*wifi_event_sta_disconnected = *((wifi_event_sta_disconnected_t *)event_data);
			printf("WIFI_EVENT_STA_DISCONNECTED, reason code %d\n", wifi_event_sta_disconnected->reason);

			if (g_retry_number < MAX_CONNECTION_RETRIES)
			{
				esp_wifi_connect();
				g_retry_number++;
			}
			else
			{
				wifi_app_send_message(WIFI_APP_MSG_STA_DISCONNECTED);
			}

			break;
		}
	}
	else if (event_base == IP_EVENT)
	{
		// case of IP event
		switch (event_id)
		{
		case IP_EVENT_STA_GOT_IP:
			ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");

			wifi_app_send_message(WIFI_APP_MSG_STA_CONNECTED_GOT_IP);
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
			break;
		}
	}
}

/**
<<<<<<< HEAD
 * Initializes the WiFi application event handler for WiFi and IP events.
 */
static void wifi_app_event_handler_init(void) {
	// Event loop for the WiFi driver
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Event handler for the connection
	esp_event_handler_instance_t instance_wifi_event;
	esp_event_handler_instance_t instance_ip_event;
	ESP_ERROR_CHECK(
			esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_wifi_event));
	ESP_ERROR_CHECK(
			esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL, &instance_ip_event));
}

/**
 * Initializes the TCP stack and default WiFi configuration.
 */
static void wifi_app_default_wifi_init(void) {
	// Initialize the TCP stack
	ESP_ERROR_CHECK(esp_netif_init());

	// Default WiFi config - operations must be in this order!
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	esp_netif_sta = esp_netif_create_default_wifi_sta();
	esp_netif_ap = esp_netif_create_default_wifi_ap();
}

/**
 * Configures the WiFi access point settings and assigns the static IP to the SoftAP.
 */
static void wifi_app_soft_ap_config(void) {
	// SoftAP - WiFi access point configuration
	wifi_config_t ap_config = { .ap = { .ssid = WIFI_AP_SSID, .ssid_len =
			strlen(WIFI_AP_SSID), .password = WIFI_AP_PASSWORD, .channel =
			WIFI_AP_CHANNEL, .ssid_hidden = WIFI_AP_SSID_HIDDEN, .authmode =
			WIFI_AUTH_WPA2_PSK, .max_connection = WIFI_AP_MAX_CONNECTIONS,
			.beacon_interval = WIFI_AP_BEACON_INTERVAL, }, };

	// Configure DHCP for the AP
	esp_netif_ip_info_t ap_ip_info;
	memset(&ap_ip_info, 0x00, sizeof(ap_ip_info));

	esp_netif_dhcps_stop(esp_netif_ap);				///> must call this first
	inet_pton(AF_INET, WIFI_AP_IP, &ap_ip_info.ip);	///> Assign access point's static IP, GW, and netmask
	inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
	inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);
	ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &ap_ip_info));///> Statically configure the network interface
	ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap));///> Start the AP DHCP server (for connecting stations e.g. your mobile device)

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));///> Setting the mode as Access Point / Station Mode
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));///> Set our configuration
	ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH));	///> Our default bandwidth 20 MHz
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE));///> Power save set to "NONE"

}

/**
 * Main task for the WiFi application
 * @param pvParameters parameter which can be passed to the task
 */
static void wifi_app_task(void *pvParameters) {
=======
 * Initialize the WIFI application event handler for WIFI and IP events
 */
static void wifi_app_event_handler_init()
{
	// Event loop for the connection
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	// Event handler for the connection
	esp_event_handler_instance_t event_instance_wifi;
	esp_event_handler_instance_t event_instance_ip;

	// Register the events to WIFI event instance
	ESP_ERROR_CHECK(
		esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
											&wifi_app_event_handler, NULL, &event_instance_wifi));

	// Register the events to IP event instance
	ESP_ERROR_CHECK(
		esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID,
											&wifi_app_event_handler, NULL, &event_instance_ip));
}

/**void *pvParameters
 * Main task for WIFI application
 */
static void wifi_app_task(void *pvParameters)
{
	// Store a message from Queue message
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
	wifi_app_queue_message_t msg;

	// Initialize the event handler
	wifi_app_event_handler_init();

<<<<<<< HEAD
	// Initialize the TCP/IP stack and WiFi config
	wifi_app_default_wifi_init();

	// SoftAP config
	wifi_app_soft_ap_config();

	// Start WiFi
	ESP_ERROR_CHECK(esp_wifi_start());

	// Send first event message
	wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SERVER);

	for (;;) {
		if (xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY)) {
			switch (msg.msgID) {
			case WIFI_APP_MSG_START_HTTP_SERVER:
				ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");

				http_server_start();
				rgb_led_http_server_started();

=======
	// Initialize the TCP/IP stack and WIFI config
	wifi_app_default_wifi_init();

	// SoftAP con-fig
	wifi_app_soft_ap_config();

	// Start WIFI
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "WIFI AP CONNECTED SUCCESSFULLY!");

	// Send test to event message
	wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SERVER);

	// Process when have the message
	for (;;)
	{
		if (xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY))
		{
			// Case of receive the message from queue -> store into msg
			ESP_LOGI(TAG, " - Received the WIFI MESSAGE - ");
			switch (msg.msgId)
			{
			case WIFI_APP_MSG_START_HTTP_SERVER:
				ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");
				// rgb_led_http_server_started();
				http_server_start();
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
				break;

			case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
				ESP_LOGI(TAG, "WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER");
<<<<<<< HEAD

=======
				// Attempt a connection
				wifi_app_connect_sta();

				// Set current number of retries to zero
				g_retry_number = 0;

				// Let the HTTP server know about the connection attempt
				http_server_monitor_send_message(HTTP_MSG_WIFI_CONNECT_INIT);
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
				break;

			case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
				ESP_LOGI(TAG, "WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
<<<<<<< HEAD
				rgb_led_wifi_connected();

				break;

			default:
				break;

=======

				// rgb_led_wifi_connected();
				http_server_monitor_send_message(HTTP_MSG_WIFI_CONNECT_SUCCESS);

				break;
			case WIFI_APP_MSG_STA_DISCONNECTED:
				ESP_LOGI(TAG, "WIFI_APP_MSG_STA_DISCONNECTED");

				http_server_monitor_send_message(HTTP_MSG_WIFI_CONNECT_FAIL);

				break;
			default:
				break;
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
			}
		}
	}
}

<<<<<<< HEAD
BaseType_t wifi_app_send_message(wifi_app_message_e msgID) {
	wifi_app_queue_message_t msg;
	msg.msgID = msgID;
	return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

void wifi_app_start(void) {
	ESP_LOGI(TAG, "STARTING WIFI APPLICATION");

	// Start WiFi started LED
	rgb_led_wifi_app_started();

	// Disable default WiFi logging messages
	esp_log_level_set("wifi", ESP_LOG_NONE);

	// Create message queue
	wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_message_t));

	// Start the WiFi application task
	xTaskCreatePinnedToCore(&wifi_app_task, "wifi_app_task",
			WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL,
			WIFI_APP_TASK_CORE_ID);
}

=======
/**
 * Entry start
 */
void wifi_app_start()
{
	ESP_LOGI(TAG, "STARTING WIFI APPLICATION");

	// RGB led display the color to indicate
	rgb_led_wifi_app_started();

	// Display default WIFI logging messages
	esp_log_level_set("WIFI", ESP_LOG_NONE);

	// Allocate memory for the wifi configuration
	wifi_config = (wifi_config_t *)malloc(sizeof(wifi_config_t));
	memset(wifi_config, 0x00, sizeof(wifi_config_t));

	// Create Message Queue
	wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_message_t));

	// Start the WIFI Application task
	xTaskCreatePinnedToCore(&wifi_app_task, "WIFI_APP_TASK",
							WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL,
							WIFI_APP_TASK_CORE_ID);
}

wifi_config_t *wifi_app_get_wifi_config(void)
{
	return wifi_config;
}
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
