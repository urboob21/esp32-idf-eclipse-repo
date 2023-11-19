/*
 * http_server.c
 *
 *  Created on: Nov 19, 2023
 *      Author: Phong Nguyen
 */
#include "esp_log.h"
#include "esp_http_server.h"

#include "freertos/queue.h"

#include "http_server.h"
#include "tasks_common.h"

static const char TAG[] = "___HTTP_SERVER___";

// HTTP Server task handle freeRTOS
static TaskHandle_t tHandler_http_server_monitor = NULL;

// HTTP Server task handle - instance of HTTP Server
static httpd_handle_t http_server_handle = NULL;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t http_server_monitor_queue_handle = NULL;

// Embedded file - define the start - end Address
extern const uint8_t jquery_3_3_1_min_js_start[] asm("_binary_jquery_3_3_1_min_js_start");
extern const uint8_t jquery_3_3_1_min_js_end[] asm("_binary_jquery_3_3_1_min_js_end");

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

extern const uint8_t app_js_start[] asm("_binary_app_js_start");
extern const uint8_t app_js_end[] asm("_binary_app_js_end");

extern const uint8_t app_css_start[] asm("_binary_app_css_start");
extern const uint8_t app_css_end[] asm("_binary_app_css_end");

extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");

/**
 * HTTP server monitor handle used to track events of the HTTP Server
 */
static void http_server_monitor() {
	http_server_queue_message_t msg;

	for (;;) {
		if (xQueueReceive(http_server_monitor_queue_handle, &msg,
		portMAX_DELAY)) {
			ESP_LOGI(TAG, "Received the MESSAGE !");
			switch (msg.msgId) {
			case HTTP_MSG_WIFI_CONNECT_INIT:
				ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_INIT");
				break;

			case HTTP_MSG_WIFI_CONNECT_SUCCESS:
				ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");
				break;

			case HTTP_MSG_WIFI_CONNECT_FAIL:
				ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_FAIL");
				break;
			default:
				break;
			}
		}
	}
}

/**
 * JQuery get handler is requested when accessing the web page
 */
static esp_err_t http_server_jquery_handler(httpd_req_t *r) {
	ESP_LOGI(TAG, "JQuery requested");

	httpd_resp_set_type(r, "application/javascript");
	httpd_resp_send(r, (const char*) jquery_3_3_1_min_js_start,
			jquery_3_3_1_min_js_end - jquery_3_3_1_min_js_start);

	return ESP_OK;
}

/**
 * Send index.html page
 */
static esp_err_t http_server_html_handler(httpd_req_t *r) {
	ESP_LOGI(TAG, "HTML requested");

	httpd_resp_set_type(r, "text/html");
	httpd_resp_send(r, (const char*) index_html_start,
			index_html_end - index_html_start);

	return ESP_OK;
}

/**
 * app.css get handler is requested when accessing the web page
 */
static esp_err_t http_server_css_handler(httpd_req_t *r) {
	ESP_LOGI(TAG, "CSS requested");

	httpd_resp_set_type(r, "text/css");
	httpd_resp_send(r, (const char*) app_css_start, app_css_end - app_css_start);

	return ESP_OK;
}

/**
 * app.js get handler is requested when accessing the web page
 */
static esp_err_t http_server_js_handler(httpd_req_t *r) {
	ESP_LOGI(TAG, "JS requested");

	httpd_resp_set_type(r, "appication/javascript");
	httpd_resp_send(r, (const char*) app_js_start, app_js_end - app_js_start);

	return ESP_OK;
}

static esp_err_t http_server_ico_handler(httpd_req_t *r) {
	ESP_LOGI(TAG, "ICO requested");

	httpd_resp_set_type(r, "image/x-icon");
	httpd_resp_send(r, (const char*) favicon_ico_start,
			favicon_ico_end - favicon_ico_start);

	return ESP_OK;
}
/**
 * Sets up the default httpd server configuration.
 * @return http server instance handle if successful, NULL otherwise.
 */
static httpd_handle_t http_server_configure() {
	// Default configuration
	httpd_config_t http_config = HTTPD_DEFAULT_CONFIG();
	http_config.core_id = HTTP_SERVER_TASK_CORE_ID;
	http_config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;
	http_config.task_priority = HTTP_SERVER_TASK_PRIORITY;
	// Increase uri handlers
	http_config.max_uri_handlers = 20;
	// Increase the timeout limits
	http_config.recv_wait_timeout = 10;
	http_config.send_wait_timeout = 10;

	ESP_LOGI(TAG,
			"http_server_configure: Starting server on port: '%d' with task priority: '%d'",
			http_config.server_port, http_config.task_priority);

	// Create HTTP Server Monitor task
	xTaskCreatePinnedToCore(&http_server_monitor, "HTTP_SERVER_MONITOR",
	HTTP_SERVER_MONITOR_STACK_SIZE, NULL, HTTP_SERVER_MONITOR_PRIORITY,
			&tHandler_http_server_monitor, HTTP_SERVER_MONITOR_CORE_ID);
	// Create the message queue
	http_server_monitor_queue_handle = xQueueCreate(3,
			sizeof(http_server_queue_message_t));

	// Start the HTTP server
	// If Instance created successfully
	if (httpd_start(&http_server_handle, &http_config) == ESP_OK) {

		ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");
		// Register HTTP Server x URIs
		// Register query handler
		httpd_uri_t jquery_js = { .uri = "/jquery-3.3.1.min.js", .method =
				HTTP_GET, .handler = http_server_jquery_handler, .user_ctx =
		NULL };
		httpd_register_uri_handler(http_server_handle, &jquery_js);

		// Register index.html handler
		httpd_uri_t index_html = { .uri = "/", .method = HTTP_GET, .handler =
				http_server_html_handler, .user_ctx = NULL };
		httpd_register_uri_handler(http_server_handle, &index_html);

		// Register app.css handler
		httpd_uri_t app_css = { .uri = "/app.css", .method = HTTP_GET,
				.handler = http_server_css_handler, .user_ctx = NULL };
		httpd_register_uri_handler(http_server_handle, &app_css);

		// Register app.js handler
		httpd_uri_t app_js = { .uri = "/app.js", .method = HTTP_GET, .handler =
				http_server_js_handler, .user_ctx = NULL };
		httpd_register_uri_handler(http_server_handle, &app_js);

		// Register favicon.ico handler
		httpd_uri_t favicon_ico = { .uri = "/favicon.ico", .method = HTTP_GET,
				.handler = http_server_ico_handler, .user_ctx = NULL };
		httpd_register_uri_handler(http_server_handle, &favicon_ico);

		return http_server_handle;
	}

	return NULL;
}

/**
 * Start the HTTP Server
 */
void http_server_start() {
	if (http_server_handle == NULL) {
		http_server_handle = http_server_configure();
		ESP_LOGI(TAG,"HTTP SERVER CREATED SUCCESSFULLY !")
	}
}

/**
 * Stop the HTTP Server
 */
void http_server_stop() {
	if (http_server_handle != NULL) {
		httpd_stop(http_server_handle);
		ESP_LOGI(TAG, "STOPPING HTTP SERVER");
		http_server_handle = NULL;
	}
	if (tHandler_http_server_monitor) {
		vTaskDelete(tHandler_http_server_monitor);
		ESP_LOGI(TAG, "http_server_stop: stopping HTTP server monitor");
		tHandler_http_server_monitor = NULL;
	}
}
