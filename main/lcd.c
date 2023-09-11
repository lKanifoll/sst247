
#include "device_init.h"

//==========================================================================================================================
									//   0     1     2     3     4     5     6     7     8     9	 --	   A     C     P
static uint8_t large_1digit_code[12] = {0x3F, 0x30, 0x6D, 0x79, 0x72, 0x5B, 0x5F, 0x38, 0x7F, 0x7B, 0x40, 0x7E};
static uint8_t large_2digit_code[13] = {0xCF, 0xC0, 0xAD, 0xE9, 0xE2, 0x6B, 0x6F, 0xC8, 0xEF, 0xEB, 0x20,       0x0F, 0xAE};
									//   0     1     2     3     4     5     6     7     8     9	 --	   o 	 C 	   \o     r
static uint8_t small_digit_code[15]  = {0xF3, 0x03, 0xB5, 0x97, 0x47, 0xD6, 0xF6, 0x13, 0xF7, 0xD7, 0x04, 0x55, 0xF0, 0xA6, 0x24};
//==========================================================================================================================

void large_digits(uint16_t number)									// OUTPUT ON THE LARGE DIGITS
{
	xSemaphoreTake(xLCD_data_using_semaphore, portMAX_DELAY);
	{
		lcd_data_buff[11] &= ~0x7F;
		lcd_data_buff[10] &= ~0xEF;

		if((number / 100) == 0)
		{
			lcd_data_buff[11] |= large_1digit_code[number / 10];
			lcd_data_buff[10] |= large_2digit_code[number % 10];
		}
		else
		{
			lcd_data_buff[11] |= large_1digit_code[number / 100];
			lcd_data_buff[10] |= large_2digit_code[number % 100];
		}
		xQueueSend(xlcd_data_queue, lcd_data_buff, portMAX_DELAY);
	}
	xSemaphoreGive(xLCD_data_using_semaphore);
}

void large_digits_blank()											// BLANK LARGE DIGITS
{
	xSemaphoreTake(xLCD_data_using_semaphore, portMAX_DELAY);
	{
		lcd_data_buff[11] &= ~0x7F;
		lcd_data_buff[10] &= ~0xEF;
		xQueueSend(xlcd_data_queue, lcd_data_buff, portMAX_DELAY);
	}
	xSemaphoreGive(xLCD_data_using_semaphore); 
}

void small_digit(uint8_t digit, uint8_t data)  						// OUTPUT ON THE SMALL DIGITS
{
	xSemaphoreTake(xLCD_data_using_semaphore, portMAX_DELAY);
	{
		lcd_data_buff[digit] &= ~0xF7;
		lcd_data_buff[digit] |= small_digit_code[data];

		xQueueSend(xlcd_data_queue, lcd_data_buff, portMAX_DELAY);
	}
	xSemaphoreGive(xLCD_data_using_semaphore);  
}

void small_digit_blank(uint8_t digit)  								// BLANK SMALL DIGIT
{
	xSemaphoreTake(xLCD_data_using_semaphore, portMAX_DELAY);
	{
		lcd_data_buff[digit] &= ~0xF7;

		xQueueSend(xlcd_data_queue, lcd_data_buff, portMAX_DELAY);
	}
	xSemaphoreGive(xLCD_data_using_semaphore);  
}

void temperature_set_large(uint8_t t_set)							// OUTPUT OF THE TEMPERATURE SET ON THE LARGE DIGITS
{
	large_digits(t_set);
	symbol_on(ICON_CELSIUS);
}

void floor_air_temperature_small()									// FLOOR AND AIR TEMPERATURE ON THE SMALL DIGITS
{
	if(sensor_set & 0x02)
	{
		if((floor_temperature / 10) == 0)
		{
			lcd_data_buff[2] &= 0x00;
		}
		else
		{
			small_digit(2, (floor_temperature / 10));
		}
		small_digit(3, (floor_temperature % 10));
		small_digit(4, 11);
		small_digit(5, 12);
	}
	else // -- --
	{
		small_digit_blank(2);
		small_digit_blank(3);
		small_digit_blank(4);
		small_digit_blank(5);			
	}

	if(sensor_set & 0x01)
	{
		if((air_temperature / 10) == 0)
		{
			lcd_data_buff[6] &= 0x00;
		}
		else
		{
			small_digit(6, (air_temperature / 10));
		}
		small_digit(7, (air_temperature % 10));
		small_digit(8, 11);
		small_digit(9, 12);
	}
	else // -- --
	{
		small_digit_blank(6);
		small_digit_blank(7);
		small_digit_blank(8);
		small_digit_blank(9);			
	}
}

void date_time_small()												// DATE OUTPUT TO SMALL DIGITS
{
	small_digit(2, RTC_TimeStr.RTC_Hours / 10);
	small_digit(3, RTC_TimeStr.RTC_Hours % 10);
	symbol_on(ICON_COLON);
	small_digit(4, RTC_TimeStr.RTC_Minutes / 10);
	small_digit(5, RTC_TimeStr.RTC_Minutes % 10);

	small_digit(6, RTC_DateStr.RTC_Date / 10);
	small_digit(7, RTC_DateStr.RTC_Date % 10);
	symbol_on(ICON_DOT);
	small_digit(8, RTC_DateStr.RTC_Month / 10);
	small_digit(9, RTC_DateStr.RTC_Month % 10);
}

void datetime_temperature()											// DATE OR TIME OUTPUT TO SMALL DIGITS
{
	if(datetime_temperature_fl)
	{
		date_time_small();
	}
	else
	{
		symbol_off(ICON_DOT);
		symbol_off(ICON_COLON);
		floor_air_temperature_small();
	}
}

void symbol_on(uint8_t Sim)											// TURN ON SYMBOLS ON THE LCD
{
	xSemaphoreTake(xLCD_data_using_semaphore, portMAX_DELAY);
	{
		switch (Sim)
		{
		case ICON_WD1:
			lcd_data_buff[0]	 |= 0x20;
			break;
		case ICON_WD2:
			lcd_data_buff[0]	 |= 0x40;
			break;
		case ICON_WD3:
			lcd_data_buff[0]	 |= 0x10;
			break;
		case ICON_WD4:
			lcd_data_buff[12]	 |= 0x80;
			break;
		case ICON_WD5:
			lcd_data_buff[12]	 |= 0x20;
			break;
		case ICON_WD6:
			lcd_data_buff[12]	 |= 0x40;
			break;
		case ICON_WD7:
			lcd_data_buff[12]	 |= 0x10;
			break;
		case ICON_SUN:
			lcd_data_buff[12]	 |= 0x02;
			break;
		case ICON_MOON:
			lcd_data_buff[12]	 |= 0x04;
			break;
		case ICON_WAKE:
			lcd_data_buff[13]	 |= 0x10;
			break;
		case ICON_LEAVE:
			lcd_data_buff[13]	 |= 0x40;
			break;
		case ICON_RETURN:
			lcd_data_buff[13]	 |= 0x20;
			break;
		case ICON_SLEEP:
			lcd_data_buff[13]	 |= 0x80;
			break;
		case ICON_KEY:
			lcd_data_buff[11]	 |= 0x80;
			break;
		case ICON_CELSIUS:
			lcd_data_buff[12]	 |= 0x08;
			break;
		case ICON_HAND:
			lcd_data_buff[0]	 |= 0x08;
			break;
		case ICON_CLOCK:
			lcd_data_buff[0]	 |= 0x02;
			break;
		case ICON_HEAT:
			lcd_data_buff[0]	 |= 0x04;
			break;
		case ICON_SNOW:
			lcd_data_buff[0]	 |= 0x01;
			break;
		case ICON_FAN:
			lcd_data_buff[1]	 |= 0x80;
			break;
		case ICON_ALARM:
			lcd_data_buff[1]	 |= 0x08;
			break;
		case ICON_LEVEL1:
			lcd_data_buff[1]	 |= 0x20;
			break;
		case ICON_LEVEL2:
			lcd_data_buff[1]	 |= 0x40;
			break;
		case ICON_LEVEL3:
			lcd_data_buff[1]	 |= 0x10;
			break;
		case ICON_HAT:
			lcd_data_buff[1]	 |= 0x04;
			break;
		case ICON_FLOR:
			lcd_data_buff[1]	 |= 0x01;
			break;
		case ICON_AIR:
			lcd_data_buff[10]	 |= 0x10;
			break;
		case ICON_COLON:
			lcd_data_buff[3]	 |= 0x08;
			break;
		case ICON_DOT:
			lcd_data_buff[7]	 |= 0x08;
			break;
		}
		xQueueSend(xlcd_data_queue, lcd_data_buff, portMAX_DELAY);
	}
	xSemaphoreGive(xLCD_data_using_semaphore);
}

void symbol_off(uint8_t Sim)										// TURN OFF SYMBOLS ON THE LCD
{
	xSemaphoreTake(xLCD_data_using_semaphore, portMAX_DELAY);
	{
		switch (Sim)
		{
		case ICON_WD1:
			lcd_data_buff[0]	 &= ~0x20;
			break;
		case ICON_WD2:
			lcd_data_buff[0]	 &= ~0x40;
			break;
		case ICON_WD3:
			lcd_data_buff[0]	 &= ~0x10;
			break;
		case ICON_WD4:
			lcd_data_buff[12]	 &= ~0x80;
			break;
		case ICON_WD5:
			lcd_data_buff[12]	 &= ~0x20;
			break;
		case ICON_WD6:
			lcd_data_buff[12]	 &= ~0x40;
			break;
		case ICON_WD7:
			lcd_data_buff[12]	 &= ~0x10;
			break;
		case ICON_SUN:
			lcd_data_buff[12]	 &= ~0x02;
			break;
		case ICON_MOON:
			lcd_data_buff[12]	 &= ~0x04;
			break;
		case ICON_WAKE:
			lcd_data_buff[13]	 &= ~0x10;
			break;
		case ICON_LEAVE:
			lcd_data_buff[13]	 &= ~0x40;
			break;
		case ICON_RETURN:
			lcd_data_buff[13]	 &= ~0x20;
			break;
		case ICON_SLEEP:
			lcd_data_buff[13]	 &= ~0x80;
			break;
		case ICON_KEY:
			lcd_data_buff[11]	 &= ~0x80;
			break;
		case ICON_CELSIUS:
			lcd_data_buff[12]	 &= ~0x08;
			break;
		case ICON_HAND:
			lcd_data_buff[0]	 &= ~0x08;
			break;
		case ICON_CLOCK:
			lcd_data_buff[0]	 &= ~0x02;
			break;
		case ICON_HEAT:
			lcd_data_buff[0]	 &= ~0x04;
			break;
		case ICON_SNOW:
			lcd_data_buff[0]	 &= ~0x01;
			break;
		case ICON_FAN:
			lcd_data_buff[1]	 &= ~0x80;
			break;
		case ICON_ALARM:
			lcd_data_buff[1]	 &= ~0x08;
			break;
		case ICON_LEVEL1:
			lcd_data_buff[1]	 &= ~0x20;
			break;
		case ICON_LEVEL2:
			lcd_data_buff[1]	 &= ~0x40;
			break;
		case ICON_LEVEL3:
			lcd_data_buff[1]	 &= ~0x10;
			break;
		case ICON_HAT:
			lcd_data_buff[1]	 &= ~0x04;
			break;
		case ICON_FLOR:
			lcd_data_buff[1]	 &= ~0x01;
			break;
		case ICON_AIR:
			lcd_data_buff[10]	 &= ~0x10;
			break;
		case ICON_COLON:
			lcd_data_buff[3]	 &= ~0x08;
			break;
		case ICON_DOT:
			lcd_data_buff[7]	 &= ~0x08;
			break;
		}
		xQueueSend(xlcd_data_queue, lcd_data_buff, portMAX_DELAY);
	}
	xSemaphoreGive(xLCD_data_using_semaphore);
}

void clear_LCD()													// CLEAR LCD
{
	xSemaphoreTake(xLCD_data_using_semaphore, portMAX_DELAY);
	{
		memset(&lcd_data_buff, 0x00, LCD_BUFF_SIZE); // ZERO MAIN LCD DATA BUFFER
		xQueueSend(xlcd_data_queue, lcd_data_buff, portMAX_DELAY);
	}
	xSemaphoreGive(xLCD_data_using_semaphore);
}

void test_LCD()														// TEST LCD
{
	xSemaphoreTake(xLCD_data_using_semaphore, portMAX_DELAY);
	{
		memset(&lcd_data_buff, 0xff, LCD_BUFF_SIZE); 
		xQueueSend(xlcd_data_queue, lcd_data_buff, portMAX_DELAY);
	}
	xSemaphoreGive(xLCD_data_using_semaphore);
}

void lcd_redraw()													// REDRAW DISPLAY
{	
	for (uint8_t i = 0; i < 7; i++)
	{
		if(i == RTC_DateStr.RTC_WeekDay)
		{
			if (work_weekend[i])
			{
				symbol_on(ICON_SUN);
			}
			else
			{
				symbol_off(ICON_SUN);
			}
			symbol_on(ICON_WD1 + RTC_DateStr.RTC_WeekDay);
		}
		else
		{
			symbol_off(ICON_WD1 + i);
		}
	}

	if(device_status == DEVICE_SCREEN_LOCK)
	{
		symbol_on(ICON_KEY);
	}
	else
	{
		symbol_off(ICON_KEY);
	}

	if(relay_flag)
	{
		symbol_on(ICON_HEAT);
	}
	else
	{
		symbol_off(ICON_HEAT);		
	}

	if(sensor_set&1)
	{
		symbol_on(ICON_AIR);
	}
 	else
	{
		symbol_off(ICON_AIR);
	} 

	if(sensor_set&2)
	{
		symbol_on(ICON_FLOR);
	}  
	else
	{
		symbol_off(ICON_FLOR);
	}

	if((sensor_set&4)&&(sensor_set&2))
	{
		symbol_on(ICON_HAT);
	}  
 	else
	{
		symbol_off(ICON_HAT);
	} 

	if (heat_mode == PROGRAM_MODE)
	{
		symbol_off(ICON_HAND);
		symbol_off(ICON_SNOW);
		symbol_on(ICON_CLOCK);
		
		if (work_weekend[RTC_DateStr.RTC_WeekDay])
		{
			symbol_off(ICON_SUN);
		}
		else
		{
			symbol_off(ICON_SUN);
		}

		temperature_set_large(set_floor_temperature_prog);
		//temperature_set_large(heat_schedule[Day_Event_Now][Event_now]);
		switch(Event_now)
		{
		case 0:
			symbol_on(ICON_WAKE);
			symbol_off(ICON_LEAVE);
			symbol_off(ICON_RETURN);
			symbol_off(ICON_SLEEP);
			break;

		case 1:
			symbol_off(ICON_WAKE);
			symbol_on(ICON_LEAVE);
			symbol_off(ICON_RETURN);
			symbol_off(ICON_SLEEP);
			break;

		case 2:
			symbol_off(ICON_WAKE);
			symbol_off(ICON_LEAVE);
			symbol_on(ICON_RETURN);
			symbol_off(ICON_SLEEP);		
			break;

		case 3:
			symbol_off(ICON_WAKE);
			symbol_off(ICON_LEAVE);
			symbol_off(ICON_RETURN);
			symbol_on(ICON_SLEEP);		
			break;
		}
	}
	else
	{
		symbol_off(ICON_CLOCK);
		symbol_off(ICON_WAKE);
		symbol_off(ICON_LEAVE);
		symbol_off(ICON_RETURN);
		symbol_off(ICON_SLEEP);	
	}

	if(heat_mode == MANUAL_MODE)
	{
		if (sensor_set & 2)
		{
			temperature_set_large(set_manual_floor_temperature);
		}
		else
		{
			temperature_set_large(set_air_temperature);
		}
		symbol_on(ICON_HAND);
		symbol_off(ICON_SNOW);
	}

	if(heat_mode == ANTIFREEZE_MODE)
	{
		temperature_set_large(set_antifreeze_floor_temperature);
		symbol_on(ICON_SNOW);
		symbol_off(ICON_HAND);
	}

	datetime_temperature();
}

void name_fw()
{
	for(uint8_t s_d = 2; s_d < 9; s_d++)
	{
		small_digit_blank(s_d);
	}
	small_digit(7, (atoi(VERSION)/100));
	small_digit(8, ((atoi(VERSION)%100)/10));
	small_digit(9, ((atoi(VERSION)%100)%10));
	symbol_on(ICON_DOT);
}

void event_time_small(uint8_t event_0_3, bool work_we)
{
	for (uint8_t k = 0; k < 7; k++) // finding day
	{
		if ((work_we) && (work_weekend[k])) // if weekend
		{
			small_digit(2,  heat_schedule[k][event_0_3].event_time / 60 / 10);
			small_digit(3,  heat_schedule[k][event_0_3].event_time / 60 % 10);
			small_digit(4, (heat_schedule[k][event_0_3].event_time - heat_schedule[k][event_0_3].event_time / 60 * 60) / 10);
			small_digit(5, (heat_schedule[k][event_0_3].event_time - heat_schedule[k][event_0_3].event_time / 60 * 60) % 10);
			break;
		}
		else if ((!work_we) && (!work_weekend[k])) // if work day
		{
			small_digit(2,  heat_schedule[k][event_0_3].event_time / 60 / 10);
			small_digit(3,  heat_schedule[k][event_0_3].event_time / 60 % 10);
			small_digit(4, (heat_schedule[k][event_0_3].event_time - heat_schedule[k][event_0_3].event_time / 60 * 60) / 10);
			small_digit(5, (heat_schedule[k][event_0_3].event_time - heat_schedule[k][event_0_3].event_time / 60 * 60) % 10);
			break;
		}
	}
}

void sens_type_to_LCD(uint8_t sensor_type)					// OUTPUT OF ANY TYPES OF SENSORS ON THE LCD
{
	switch (sensor_type)
	{
	case 1:
		small_digit(7, 6);
		small_digit(8, 8);
		symbol_on(ICON_DOT);
		break;
	case 2:
		small_digit(6, 1);
		small_digit(7, 0);
		break;
	case 3:
		small_digit(6, 1);
		small_digit(7, 2);
		break;
	case 4:
		small_digit(6, 1);
		small_digit(7, 5);

		break;
	case 5:
		small_digit(6, 3);
		small_digit(7, 3);
		break;
	case 6:
		small_digit(6, 4);
		small_digit(7, 7);
		break;
	}
}