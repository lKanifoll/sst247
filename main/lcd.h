#ifndef LCD_H_
#define LCD_H_

#include "device_init.h"


uint8_t lcd_data_buff[LCD_BUFF_SIZE];			// MAIN LCD DATA BUFFER

QueueHandle_t xlcd_data_queue;					// HANDLE FOR LCD DATA QUEUE
SemaphoreHandle_t xLCD_data_using_semaphore;	// SEMAPHORE HANDLE FOR SEPARATE USE MAIN LCD BUFFER TO PREVENT SIMULTANEOUSLY ACCESS

typedef enum
{
	ICON_WD1,			//	icon "day 1"
	ICON_WD2,			//	icon "day 2"
	ICON_WD3,			//	icon "day 3"
	ICON_WD4,			//	icon "day 4"
	ICON_WD5,			//	icon "day 5"
	ICON_WD6,			//	icon "day 6"
	ICON_WD7,			//	icon "day 7"

	ICON_SUN,			//	icon "SUN"
	ICON_MOON,			//	icon "MOON"

	ICON_WAKE,
	ICON_LEAVE,
	ICON_RETURN,
	ICON_SLEEP,

	ICON_KEY,			//	icon "lock key"
	ICON_CELSIUS,		//	icon "celsius degree"

	ICON_SNOW,			//	icon "snowflake"
	ICON_CLOCK,			//	icon "clock"
	ICON_HAND,			//	icon "manual mode"
	ICON_HEAT,			//	icon "heat"
	
	ICON_FAN,			//	icon "fan"
	ICON_ALARM,			//	icon "alarm"

	ICON_LEVEL1,		//	icon wifi level 1
	ICON_LEVEL2,		//	icon wifi level 2
	ICON_LEVEL3,		//	icon wifi level 3

	ICON_HAT,			//	icon "selfstudy"
	ICON_FLOR,			//	icon "floor sensor"
	ICON_AIR,			//	icon "air sensor"
	
	ICON_COLON,			//	icon "colon"
	ICON_DOT,			//	icon "dot"

} lcd_icons_t;


void large_digits(uint16_t number);
void large_digits_blank();
void small_digit(uint8_t digit, uint8_t data);
void small_digit_blank(uint8_t digit);
void temperature_set_large(uint8_t t_set);
void date_time_small();
void floor_air_temperature_small();
void datetime_temperature();
void symbol_on(uint8_t Sim);
void symbol_off(uint8_t Sim);
void clear_LCD();
void test_LCD();
void lcd_redraw();
void name_fw();
void event_time_small(uint8_t event_0_3, bool work_we);
void sens_type_to_LCD(uint8_t sensor_type);
#endif