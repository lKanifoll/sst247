#ifndef DEVICE_INIT_H_
#define DEVICE_INIT_H_

#include <errno.h>
#include "esp_types.h"
#include "driver/gpio.h"
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include "driver/adc.h"
#include <driver/i2c.h>
#include <driver/adc.h>
#include "esp_adc_cal.h"
#include "soc/sens_reg.h"
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include <freertos/queue.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_ota_ops.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <sys/socket.h>
#include <netdb.h>
#include <newlib.h>
#include "freertos/event_groups.h"
#include "esp_smartconfig.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/netbuf.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "rom/ets_sys.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "driver/touch_pad.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

//---------------------------------------
#include "defines.h"
#include "workflow.h"
#include "pcf85162.h"
#include "pcf85063.h"
#include "lcd.h"
#include "wifi_routine.h"
#include "tcp_mqtt.h"
#include "ota_update.h"
//---------------------------------------
//========================================
typedef struct
{
    uint16_t event_time;
    uint8_t temperature;
} event_cell;

event_cell heat_schedule[7][4];
uint8_t work_weekend[7];
//========================================

TaskHandle_t connect_task_handle;
TaskHandle_t xTask_read_temp_handle;
TaskHandle_t xTask_interface_handle;


esp_adc_cal_characteristics_t *adc_chars;

nvs_handle my_handle;
uint8_t small_output_timer;
uint8_t reconnect_flag;
uint8_t first_start;
uint8_t device_status;
uint8_t blinking;
uint16_t calib_vref;
uint16_t full_year;
uint8_t conf_step;
uint8_t device_power_status;
uint8_t sensor_err;
uint8_t air_temperature;
uint8_t floor_temperature;
uint8_t set_floor_temperature_prog;
uint8_t relay_flag;
uint8_t device_return_mode;
uint8_t week_byte;

uint16_t relay_operating_time_tmp;
uint16_t relay_time_ours;
uint8_t relay_time_minutes;

uint8_t bkl_led_off_timer;
uint8_t datetime_temperature_fl;

uint8_t flag_preheat;
uint8_t preheat_begin_temperature;

//uint16_t touch_init_val[TOUCH_PAD_MAX];
uint8_t  touch_button_activated[10];
uint8_t  touch_pad_activated[10];
uint16_t touch_button_no_use[TOUCH_PAD_MAX];
uint16_t touch_value;
uint16_t counter_calib;

//======================================== To flash
uint8_t preheat_timetable[40];
uint8_t heat_mode;
uint8_t set_prog_comfort_temp;
uint8_t set_prog_eco_temp;
uint8_t set_manual_floor_temperature;
uint8_t set_antifreeze_floor_temperature;
uint8_t set_air_temperature;
uint8_t love_temperature;
uint8_t delta_air_temperature;
uint8_t sensor_set;
uint8_t sensor_type;
uint8_t first_link;
uint8_t SSID[32];
uint8_t PASS[32];
uint8_t UID[11];
uint8_t sensor_err;

char MQTT_Port[4];
char MQTT_IP[64];

uint32_t relay_operating_time;
//========================================




void beep_init(uint32_t lcd_duty);
void beep_beep();
void device_gpio_init();
void adc_init();
void set_factory_settings(void);
void load_from_flash(void);
void init_i2c_rtc();
//void connect_task(void *arg);
void connect_task(void *arg);
void touch_set_thresholds(void);
void touchinit(void);
void tp_example_read_task(void *pvParameter);

#endif