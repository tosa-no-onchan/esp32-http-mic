/*
 * app_httpd_sim.h
 *
 *  Created on: 2020/09/08
 *      Author: nishi
 */

#ifndef MAIN_APP_HTTPD_MIC_H_
#define MAIN_APP_HTTPD_MIC_H_

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"

#include <esp_http_server.h>


void connect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
void disconnect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);

httpd_handle_t start_webserver(void);


#endif /* MAIN_APP_HTTPD_MIC_H_ */
