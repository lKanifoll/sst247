#ifndef WIFI_ROUTINE_H_
#define WIFI_ROUTINE_H_

#include "device_init.h"

struct T_instruction
{
    uint8_t ID;
    uint16_t length;
    uint16_t CRC;
};

wifi_ap_record_t ap_info;

xTaskHandle smartconfig_handle;
xTaskHandle mqtt_publish_handle;
xTaskHandle mqtt_subscribe_handle;
xTaskHandle tcp_server_handle;
xTaskHandle udp_server_handle;
xTaskHandle connect_wifi_handle;

uint8_t Access_flag;
uint8_t conn_flag;
uint8_t ap_flag;
uint8_t first_link;
uint8_t task_flag;
uint8_t WIFI;
uint8_t wifi_start_flag;
uint8_t disconnect;

uint8_t curr_interface;
uint8_t sta_ap_flag;
uint8_t temp_mac[6];
uint8_t WIFI_Level;


char MAC_esp[18];

void initialise_wifi(void);
void smartconfig_example_task(void *parm);
void set_wifi_sta(void);
void set_wifi_ap(void);
void reconnect_wifi(void);
void wifi_rssi();

#endif