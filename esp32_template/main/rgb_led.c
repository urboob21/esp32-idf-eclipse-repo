/*
 * rgb_led.c
 *
 *  Created on: Oct 28, 2023
 *      Author: Phong Nguyen
 */

#include <stdbool.h>
#include <stdio.h>
/**
 * Indicate Wifi application has started
 */
void rgb_led_wifi_app_started(void) {
	printf("Wifi started\n");
}

/**
 * Indicate HTTP application has started
 */
void rgb_led_http_app_started(void) {
	printf("Http started\n");
}

/**
 * Indicate connection to an acess point
 */
void rgb_led_wifi_connected(void) {
	printf("Wifi connected\n");
}
