/*
 * http_server.h
 *
 *  Created on: Nov 19, 2023
 *      Author: Phong Nguyen
 */

#ifndef MAIN_HTTP_SERVER_H_
#define MAIN_HTTP_SERVER_H_

/**
 * Message for the HTTP Server
 */
typedef enum {
	HTTP_MSG_WIFI_CONNECT_INIT=0,
	HTTP_MSG_WIFI_CONNECT_SUCCESS,
	HTTP_MSG_WIFI_CONNECT_FAIL
}http_server_message_id_e;


/**
 * Structure for the message queue
 */
typedef struct{
	http_server_message_id_e msgId;
} http_server_queue_message_t;

/**
 * Start the HTTP Server
 */
void http_server_start();

/**
 * Stop the HTTP Server
 */
void http_server_stop();


#endif /* MAIN_HTTP_SERVER_H_ */
