#ifndef OTA_UPDATE_H_
#define OTA_UPDATE_H_

#include "device_init.h"

bool force_update;

xTaskHandle ota_handle;
//#define EXAMPLE_WIFI_SSID    "Keenetic-5919"
//#define EXAMPLE_WIFI_PASS    "2kcdbL5b"
#define SERVER_IP    "api.sst-cloud.com"
#define SERVER_PORT  "80"
#define BUFFSIZE 1024
#define TEXT_BUFFSIZE 1024

void ota_example_task();

#endif