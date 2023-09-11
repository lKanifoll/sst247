/*
 * smartconfig.c
 *
 *  Created on: Dec 19, 2017
 *      Author: sergey
 */

#include "wifi_routine.h"
#include "device_init.h"

static EventGroupHandle_t wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "sc";


void wifi_rssi()
{
	esp_wifi_sta_get_ap_info(&ap_info);

    if(ap_info.rssi == 0)
    {
        symbol_off(ICON_LEVEL1);
        symbol_off(ICON_LEVEL2);
        symbol_off(ICON_LEVEL3);
    }
    else if((ap_info.rssi <= (-10))&&(ap_info.rssi >= (-60)))					  
    {
        WIFI_Level = 0x04; 
        symbol_on(ICON_LEVEL1);
        symbol_on(ICON_LEVEL2);
        symbol_on(ICON_LEVEL3);
    }
	if((ap_info.rssi <= (-60))&&(ap_info.rssi >= (-70)))  
    {
        WIFI_Level = 0x03;
        symbol_on(ICON_LEVEL1);
        symbol_on(ICON_LEVEL2);
        symbol_off(ICON_LEVEL3);
    }
	if(ap_info.rssi < (-70))  							  
    {
        WIFI_Level = 0x01;     
        symbol_on(ICON_LEVEL1);
        symbol_off(ICON_LEVEL2);
        symbol_off(ICON_LEVEL3);
    }
	//printf("RSSI %d\n",ap_info.rssi);
}

void get_mac_buf()
{
    while (ESP_OK != esp_wifi_get_mac(ESP_IF_WIFI_STA, temp_mac));
    sprintf(&MAC_esp[0], "%02X:%02X:%02X:%02X:%02X:%02X", temp_mac[0], temp_mac[1], temp_mac[2], temp_mac[3], temp_mac[4], temp_mac[5]);
    printf("%s\n", MAC_esp);
}

//---------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Handle WIFI status--------------------------------------------
//---------------------------------------------------------------------------------------------------------------
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
     switch (event->event_id)
    {

    case SYSTEM_EVENT_AP_START:
        wifi_start_flag = 1;
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        ap_flag = 1;
        first_link = 1;

        if (disconnect)
        {
            vTaskResume(tcp_server_handle);
            vTaskResume(udp_server_handle);
            vTaskResume(mqtt_subscribe_handle);
            disconnect = 0;
        }
        else
        {
            xTaskCreate(&tcp_server,     "tcp_server",     4096, NULL, 10, &tcp_server_handle);
            xTaskCreate(&udp_server,     "udp_server",     4096, NULL, 10, &udp_server_handle);
            xTaskCreate(&mqtt_subscribe, "mqtt_subscribe", 8192, NULL, 10, &mqtt_subscribe_handle);
        }

        break;

    case SYSTEM_EVENT_AP_STACONNECTED:

        WIFI = 1;

        break;

    case SYSTEM_EVENT_AP_STOP:

        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        WIFI = 0;
        WIFI_Level = 0x00;
        wifi_start_flag = 0;

        vTaskSuspend(tcp_server_handle);
        vTaskSuspend(udp_server_handle);
        vTaskSuspend(mqtt_subscribe_handle);
        WIFI = 0;
        disconnect = 1;
        break;

    case SYSTEM_EVENT_STA_START:

        wifi_start_flag = 1;

        break;
    case SYSTEM_EVENT_STA_GOT_IP:

        sta_ap_flag = 0;
        LED_OFF;

        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        WIFI = 1;
        get_mac_buf();

        if (disconnect)
        {
            vTaskResume(tcp_server_handle);
            vTaskResume(udp_server_handle);
            vTaskResume(mqtt_subscribe_handle);
            disconnect = 0;
        }
        else
        {
            if(device_status != DEVICE_OFF) vTaskResume(xTask_read_temp_handle);
            xTaskCreate(&tcp_server,     "tcp_server",     4096, NULL, 10, &tcp_server_handle);
            xTaskCreate(&udp_server,     "udp_server",     4096, NULL, 10, &udp_server_handle);
            xTaskCreate(&mqtt_subscribe, "mqtt_subscribe", 8192, NULL, 10, &mqtt_subscribe_handle);
            vTaskDelay(30000 / portTICK_PERIOD_MS);
            //xTaskCreate(&ota_example_task, "ota_example_task", 4096, NULL, 20, &ota_handle);
        }
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:

        if (WIFI == 1)
        {
            vTaskSuspend(tcp_server_handle);
            vTaskSuspend(udp_server_handle);
            vTaskSuspend(mqtt_subscribe_handle);
            WIFI = 0;
            disconnect = 1;
        }

        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        WIFI_Level = 0x00;
        wifi_start_flag = 0;
        printf("DISCONNECTED\n");
        
        if(!sta_ap_flag) esp_wifi_connect();

        break;
    default:
        break;
    }
    return ESP_OK; 
}

//---------------------------------------------------------------------------------------------------------------
//-------------------------------------------------WIFI init-----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

void initialise_wifi(void)
{
    tcpip_adapter_init();

    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    nvs_open("storage", NVS_READWRITE, &my_handle);
    nvs_get_u8(my_handle, "first_link", &first_link);
    nvs_commit(my_handle);
    nvs_close(my_handle);

    if (first_link)
    {
        wifi_config_t wifi_config =
            {
                .sta =
                    {
                        .ssid = "",
                        .password = "",
                    },
            };

        strcpy((char *)&wifi_config.sta.ssid[0], (const char *)&SSID[0]);
        strcpy((char *)&wifi_config.sta.password[0], (const char *)&PASS[0]);

        //curr_interface = ESP_IF_WIFI_STA;

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        curr_interface = ESP_IF_WIFI_STA;

        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
}

void set_wifi_sta(void)
{
    sta_ap_flag = 1;

    if (wifi_start_flag)
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_stop());
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    curr_interface = ESP_IF_WIFI_STA;
    ESP_ERROR_CHECK(esp_wifi_start());
    xTaskCreate(&smartconfig_example_task, "smartconfig_example_task", 4096, NULL, 15, &smartconfig_handle);
    sta_ap_flag = 1;
}

void set_wifi_ap()
{
    sta_ap_flag = 1;
    curr_interface = ESP_IF_WIFI_STA;
    //get_mac_buf();
    if (wifi_start_flag)
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_stop());
    }
    wifi_config_t wifi_config1 = {
        .ap = {
            //.ssid="Equation",
            .ssid_len = 17,
            .password = "1234567890",
            .channel = 6,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .ssid_hidden = 0,
            .max_connection = 4,
            .beacon_interval = 100},
    };
    strncpy((char *)wifi_config1.ap.ssid, (char *)MAC_esp, 17);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config1));
    //ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
    sta_ap_flag = 1;
    ESP_ERROR_CHECK(esp_wifi_start());
}

void reconnect_wifi()
{

    //ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_stop());

    ESP_ERROR_CHECK(esp_wifi_restore());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };

    strcpy((char *)&wifi_config.sta.ssid[0], (const char *)&SSID[0]);
    strcpy((char *)&wifi_config.sta.password[0], (const char *)&PASS[0]);
    curr_interface = ESP_IF_WIFI_STA;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    curr_interface = ESP_IF_WIFI_STA;

    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}

//---------------------------------------------------------------------------------------------------------------
//-------------------------------------------------Handle smartconfig status-------------------------------------
//---------------------------------------------------------------------------------------------------------------
static void sc_callback(smartconfig_status_t status, void *pdata)
{
    switch (status)
    {
    case SC_STATUS_WAIT:
        ESP_LOGI(TAG, "SC_STATUS_WAIT");
        break;
    case SC_STATUS_FIND_CHANNEL:
        ESP_LOGI(TAG, "SC_STATUS_FINDING_CHANNEL");
        break;
    case SC_STATUS_GETTING_SSID_PSWD:
        ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD");

        break;
    case SC_STATUS_LINK:
        ESP_LOGI(TAG, "SC_STATUS_LINK");
        wifi_config_t *wifi_config = pdata;
        wifi_config->sta.bssid_set = 0;
        ESP_LOGI(TAG, "SSID:%s", wifi_config->sta.ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", wifi_config->sta.password);

        //---------------------------------------------------------------------------------------------------------------Save SSID & PASS to flash
        strcpy((char *)&SSID[0], (const char *)&wifi_config->sta.ssid[0]);
        strcpy((char *)&PASS[0], (const char *)&wifi_config->sta.password[0]);

        nvs_open("storage", NVS_READWRITE, &my_handle);
        nvs_set_str(my_handle, "SSID", (const char *)SSID);
        nvs_set_str(my_handle, "PASS", (const char *)PASS);
        nvs_commit(my_handle);
        nvs_close(my_handle);

        //---------------------------------------------------------------------------------------------------------------
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());

        break;
    case SC_STATUS_LINK_OVER:
        ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
        if (pdata != NULL)
        {
            uint8_t phone_ip[4] = {0};
            memcpy(phone_ip, (uint8_t *)pdata, 4);
            ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
        }
        xEventGroupSetBits(wifi_event_group, ESPTOUCH_DONE_BIT);
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------------------------------------------------------
//--------------------------------------------------Smartconfig task--------------------------------------------
//---------------------------------------------------------------------------------------------------------------
void smartconfig_example_task(void *parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    ESP_ERROR_CHECK(esp_smartconfig_start(sc_callback));

    while (1)
    {
        uxBits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if (uxBits & CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "WiFi Connected to ap");
            first_link = 1;

            nvs_open("storage", NVS_READWRITE, &my_handle);
            nvs_set_u8(my_handle, "first_link", first_link);
            nvs_commit(my_handle);
            nvs_close(my_handle);
        }
        if (uxBits & ESPTOUCH_DONE_BIT)
        {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}