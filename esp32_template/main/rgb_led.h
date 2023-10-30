/*
 * rgb_led.h
 *
 *  Created on: Oct 28, 2023
 *      Author: Phong Nguyen
 */

#ifndef MAIN_RGB_LED_H_
#define MAIN_RGB_LED_H_

/**
 * Indicate Wifi application has started
 */
void rgb_led_wifi_app_started(void);
void rgb_led_http_server_started(void);
/**
 * Indicate HTTP application has started
 */
void rgb_led_http_app_started(void);

/**
 * Indicate connection to an acess point
 */
void rgb_led_wifi_connected(void);

#endif /* MAIN_RGB_LED_H_ */
