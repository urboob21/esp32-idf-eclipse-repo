/*
 * tasks_common.h
<<<<<<< HEAD
 *	Define properties rtos task common
 *  Created on: Oct 29, 2023
=======
 *
 *  Created on: Nov 19, 2023
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
 *      Author: Phong Nguyen
 */

#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

<<<<<<< HEAD
// Wifi application task
#define WIFI_APP_TASK_STACK_SIZE		4096
#define WIFI_APP_TASK_PRIORITY			5
#define WIFI_APP_TASK_CORE_ID			0

// HTTP Server task
=======
// WIFI task
#define WIFI_APP_TASK_STACK_SIZE 	    4069
#define WIFI_APP_TASK_PRIORITY		    5
#define WIFI_APP_TASK_CORE_ID			0

// HTTP Server task (process from server ??)
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
#define HTTP_SERVER_TASK_STACK_SIZE			8192
#define HTTP_SERVER_TASK_PRIORITY			4
#define HTTP_SERVER_TASK_CORE_ID			0

<<<<<<< HEAD
// HTTP Server Monitor task
=======
// HTTP Server Monitor task (freeRTOS task)
>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
#define HTTP_SERVER_MONITOR_STACK_SIZE		4096
#define HTTP_SERVER_MONITOR_PRIORITY		3
#define HTTP_SERVER_MONITOR_CORE_ID			0

<<<<<<< HEAD
=======
// DHT 22 sensor task
#define DHT22_TASK_STACK_SIZE		4096
#define DHT22_TASK_PRIORITY			5
#define DHT22_TASK_CORE_ID			1

>>>>>>> 5069f425920a07546b18c01d1fd2a5acb1dbb628
#endif /* MAIN_TASKS_COMMON_H_ */
