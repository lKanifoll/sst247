#include "device_init.h"


//============================================================================================
//============================================================================= Menu routine =
//============================================================================================
uint8_t day_set    = 0;
uint8_t event_time = 0;
void device_menu(void)
{
		switch (device_status)
		{
		case DEVICE_OFF:			// DEVICE OFF ROUTINE
			if ((BT_ON_OFF_PAR.type_click == LONG_CLICK)  && (BT_ON_OFF_PAR.flag_click))
			{
				//BT_ON_OFF_PAR.type_click = NO_CLICK;
				//BT_ON_OFF_PAR.counter = 0;
				//BT_ON_OFF_PAR.flag_click = 0;
				beep_beep();
	 			bkl_led_off_timer = BKL_OFF_TIME;
				device_status = DEVICE_SCREEN_UNLOCK;
				vTaskSuspend(xTask_read_temp_handle);
				LED_ON;
				lcd_redraw();

				device_power_status = pdTRUE;
				nvs_open("storage", NVS_READWRITE, &my_handle);
				nvs_set_u8(my_handle, "ON_OFF", device_power_status);
				nvs_commit(my_handle);
				nvs_close(my_handle);
			}
			break;

		case DEVICE_SCREEN_LOCK: 	// LOCK SCREEN ROUTINE
			
			if ((BT_ON_OFF_PAR.type_click == LONG_CLICK) 
			 && (BT_ON_OFF_PAR.flag_click))
			{
				//BT_ON_OFF_PAR.type_click = NO_CLICK;
				//BT_ON_OFF_PAR.counter = 0;
				//BT_ON_OFF_PAR.flag_click = 0;
				beep_beep();
				bkl_led_off_timer = BKL_OFF_TIME;
				LED_ON;
				device_status = DEVICE_SCREEN_UNLOCK;
				vTaskSuspend(xTask_read_temp_handle);
				RELAY_OFF;
				relay_flag = 0;
				lcd_redraw();
			}
			break;

		case DEVICE_SCREEN_UNLOCK:	// MAIN ROUTINE
		
			//======================================================================================================================================================================= STA MODE
			if ((!plus_minus_flag) && (BT_PLUS_PAR.type_click == SHORT_CLICK) && (BT_PLUS_PAR.flag_click) && (BT_ON_OFF_PAR.type_click == SHORT_CLICK) && (BT_ON_OFF_PAR.flag_click))
			{
				plus_minus_flag = pdTRUE;
				bkl_led_off_timer = BKL_OFF_TIME;
			}

			if ((plus_minus_flag) && ((BT_PLUS_PAR.type_click == LONG_CLICK) && (BT_PLUS_PAR.flag_click) && (BT_ON_OFF_PAR.type_click == LONG_CLICK) && (BT_ON_OFF_PAR.flag_click)))
			{
				BT_PLUS_PAR.type_click = NO_CLICK;
				//BT_PLUS_PAR.counter = 0;
				//BT_PLUS_PAR.flag_click = 0;
				BT_ON_OFF_PAR.type_click = NO_CLICK;
				//BT_MODE_OK_PAR.counter = 0;
				//BT_MODE_OK_PAR.flag_click = 0;
				beep_beep();
				bkl_led_off_timer = 0;
				LED_ON;
				vTaskSuspend(xTask_read_temp_handle);
				clear_LCD();
				xTaskCreate(&connect_task, "connect_task", 4096, (void *)0, 10, &connect_task_handle);
				set_wifi_sta();
			}
            //======================================================================================================================================================================= AP mode

			if ((!plus_minus_flag) && (BT_MINUS_PAR.type_click == SHORT_CLICK) && (BT_MINUS_PAR.flag_click) && (BT_ON_OFF_PAR.type_click == SHORT_CLICK) && (BT_ON_OFF_PAR.flag_click))
			{
				plus_minus_flag = pdTRUE;
				bkl_led_off_timer = BKL_OFF_TIME;
			}

			if ((plus_minus_flag) && ((BT_MINUS_PAR.type_click == LONG_CLICK) && (BT_MINUS_PAR.flag_click) && (BT_ON_OFF_PAR.type_click == LONG_CLICK) && (BT_ON_OFF_PAR.flag_click)))
			{
				BT_MINUS_PAR.type_click = NO_CLICK;
				//BT_MINUS_PAR.counter = 0;
				//BT_MINUS_PAR.flag_click = 0;
				BT_ON_OFF_PAR.type_click = NO_CLICK;
				//BT_ON_OFF_PAR.counter = 0;
				//BT_ON_OFF_PAR.flag_click = 0;
				beep_beep();
				bkl_led_off_timer = 0;
				LED_ON;
				vTaskSuspend(xTask_read_temp_handle);
				clear_LCD();
				xTaskCreate(&connect_task, "connect_task", 4096, (void  *)1, 10, &connect_task_handle);
				set_wifi_ap();
			}
            //=======================================================================================================================================================================

			if ((!plus_minus_flag) && (((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))||((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))))
			{
				BT_PLUS_PAR.type_click = NO_CLICK;
				BT_PLUS_PAR.counter = 0;
				BT_PLUS_PAR.flag_click = 0;
				BT_MINUS_PAR.type_click = NO_CLICK;
				BT_MINUS_PAR.counter = 0;
				BT_MINUS_PAR.flag_click = 0;

				beep_beep();
				bkl_led_off_timer = BKL_OFF_TIME;

				device_status = DEVICE_TEMP_ADJ_MANUAL;
			}
			//=======================================================================================================================================================================

			if ((!plus_minus_flag) && (BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
			{
				BT_MODE_OK_PAR.type_click = NO_CLICK;
				BT_MODE_OK_PAR.counter = 0;
				BT_MODE_OK_PAR.flag_click = 0;

				beep_beep();
				bkl_led_off_timer = BKL_OFF_TIME;
				device_status = DEVICE_MODE_SET;
			}

			if ((!plus_minus_flag) && (BT_ON_OFF_PAR.type_click == LONG_CLICK) && (BT_ON_OFF_PAR.flag_click))
			{
				BT_ON_OFF_PAR.type_click = NO_CLICK;
				BT_ON_OFF_PAR.counter = 0;
				BT_ON_OFF_PAR.flag_click = 0;
				beep_beep();
				bkl_led_off_timer = 0;
				device_status = DEVICE_OFF;
				vTaskSuspend(xTask_read_temp_handle);
				LED_OFF;
				RELAY_OFF;
				relay_flag = 0;
				clear_LCD();
				device_power_status = pdFALSE;
				nvs_open("storage", NVS_READWRITE, &my_handle);
				nvs_set_u8(my_handle, "ON_OFF", device_power_status);
				nvs_commit(my_handle);
				nvs_close(my_handle);
			}

			if((!plus_minus_flag) && ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click)))
			{
				//BT_PLUS_PAR.counter=0;
				//BT_PLUS_PAR.flag_click=0;
				//BT_MINUS_PAR.counter=0;
				//BT_MINUS_PAR.flag_click=0;
				name_fw();
				beep_beep();
				bkl_led_off_timer = BKL_OFF_TIME;
				plus_minus_flag = pdTRUE;
			}

			if((plus_minus_flag) && ((BT_SET_CANCEL_PAR.type_click >= SHORT_CLICK) && (!BT_SET_CANCEL_PAR.flag_click)))
			{
				BT_PLUS_PAR.type_click = NO_CLICK;
				BT_PLUS_PAR.counter=0;
				BT_PLUS_PAR.flag_click=0;
				BT_MINUS_PAR.type_click = NO_CLICK;
				BT_MINUS_PAR.counter=0;
				BT_MINUS_PAR.flag_click=0;
				symbol_off(ICON_DOT);
				lcd_redraw();
				bkl_led_off_timer = BKL_OFF_TIME;
				
			}

			if((plus_minus_flag) && ((BT_SET_CANCEL_PAR.type_click >= LONG_CLICK) && (BT_SET_CANCEL_PAR.flag_click)))
			{
				// BT_PLUS_PAR.type_click = NO_CLICK;
				// BT_PLUS_PAR.counter = 0;
				// BT_PLUS_PAR.flag_click = 0;
				// BT_MINUS_PAR.type_click = NO_CLICK;
				// BT_MINUS_PAR.counter = 0;
				// BT_MINUS_PAR.flag_click = 0;
				beep_beep();
				conf_step = 1;
				device_status = DEVICE_MANUAL_CONFIG;
				clear_LCD();
			}			
			break;

		
		case DEVICE_MODE_SET:		// MODE SELECTION ROUTINE

			if (blinking == pdTRUE)
			{
				symbol_on(ICON_HAND - heat_mode);
			}
			else if (blinking == pdFALSE)
			{
				symbol_off(ICON_HAND - heat_mode);
			}

			if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
			{
				BT_PLUS_PAR.type_click = NO_CLICK;
				BT_PLUS_PAR.counter = 0;
				BT_PLUS_PAR.flag_click = 0;
				beep_beep();
				bkl_led_off_timer = BKL_OFF_TIME;
				if (heat_mode < 2)
				{
					heat_mode++;
					clear_LCD();
					lcd_redraw();
				}
			}

			if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
			{
				BT_MINUS_PAR.type_click = NO_CLICK;
				BT_MINUS_PAR.counter = 0;
				BT_MINUS_PAR.flag_click = 0;
				beep_beep();
				bkl_led_off_timer = BKL_OFF_TIME;
				if (heat_mode > 0)
				{
					heat_mode--;
					clear_LCD();
					lcd_redraw();
				}
			}

			if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
			{
				BT_MODE_OK_PAR.type_click = NO_CLICK;
				BT_MODE_OK_PAR.counter = 0;
				BT_MODE_OK_PAR.flag_click = 0;
				beep_beep();
				bkl_led_off_timer = BKL_OFF_TIME;
				device_status = DEVICE_SCREEN_UNLOCK;
				nvs_open("storage", NVS_READWRITE, &my_handle);
				nvs_set_u8(my_handle, "heat_mode", heat_mode);
				nvs_commit(my_handle);
				nvs_close(my_handle);
				symbol_on(ICON_HAND - heat_mode);
			}
			break;
		case DEVICE_TEMP_ADJ_MANUAL:// ADJUST TEMP IN MANUAL MODE

			switch (heat_mode)
			{
			case MANUAL_MODE:
			
				if (blinking == pdTRUE)
				{
					temperature_set_large(set_manual_floor_temperature);
				}
				else if (blinking == pdFALSE)
				{
					large_digits_blank();
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_manual_floor_temperature < 45)
					{
						set_manual_floor_temperature++;
						blinking = pdTRUE;
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 25;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_manual_floor_temperature < 45)
					{
						set_manual_floor_temperature++;
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_manual_floor_temperature > 5)
					{
						set_manual_floor_temperature--;
						blinking = pdTRUE;
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 25;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_manual_floor_temperature > 5)
					{
						set_manual_floor_temperature--;
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					device_status = DEVICE_SCREEN_UNLOCK;
					//temperature_set_to_lcd();
					lcd_redraw();
					nvs_open("storage", NVS_READWRITE, &my_handle);
					nvs_set_u8(my_handle, "set_floor_temp", set_manual_floor_temperature);
					nvs_commit(my_handle);
					nvs_close(my_handle);
				}
				break;
			
			case PROGRAM_MODE:
				if (blinking == pdTRUE)
				{
					temperature_set_large(set_floor_temperature_prog);
				}
				else if (blinking == pdFALSE)
				{
					large_digits_blank();
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_floor_temperature_prog < 45)
					{
						set_floor_temperature_prog++;
						blinking = pdTRUE;
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 25;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_floor_temperature_prog < 45)
					{
						set_floor_temperature_prog++;
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_floor_temperature_prog > 5)
					{
						set_floor_temperature_prog--;
						blinking = pdTRUE;
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 25;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_floor_temperature_prog > 5)
					{
						set_floor_temperature_prog--;
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					device_status = DEVICE_SCREEN_UNLOCK;
					lcd_redraw();
				}			
				break;

			case ANTIFREEZE_MODE:
				if (blinking == pdTRUE)
				{
					temperature_set_large(set_antifreeze_floor_temperature);
				}
				else if (blinking == pdFALSE)
				{
					large_digits_blank();
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_antifreeze_floor_temperature < 12)
					{
						set_antifreeze_floor_temperature++;
						blinking = pdTRUE;
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 25;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_antifreeze_floor_temperature < 12)
					{
						set_antifreeze_floor_temperature++;
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_antifreeze_floor_temperature > 5)
					{
						set_antifreeze_floor_temperature--;
						blinking = pdTRUE;
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 25;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_antifreeze_floor_temperature > 5)
					{
						set_antifreeze_floor_temperature--;
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					device_status = DEVICE_SCREEN_UNLOCK;
					lcd_redraw();
					nvs_open("storage", NVS_READWRITE, &my_handle);
					nvs_set_u8(my_handle, "set_af_temp", set_antifreeze_floor_temperature);
					nvs_commit(my_handle);
					nvs_close(my_handle);
				}			
				break;
			}
			break;

		case DEVICE_MANUAL_CONFIG:	// ROUTINE FOR MANUAL CONFIGURATION

			switch (conf_step)
			{
			case 1:		// ECO TEMP SET

				
				symbol_on(ICON_CELSIUS);
				symbol_on(ICON_CLOCK);

				if (blinking == pdTRUE)
				{
					symbol_on(ICON_MOON);
					temperature_set_large(set_prog_eco_temp);
				}
				else if (blinking == pdFALSE)
				{
					symbol_on(ICON_MOON);
					large_digits_blank();
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_prog_eco_temp < 45)
					{
						set_prog_eco_temp++;
						
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_prog_eco_temp < 45)
					{
						set_prog_eco_temp++;
						
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_prog_eco_temp > 5)
					{
						set_prog_eco_temp--;
						
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (set_prog_eco_temp > 5)
					{
						set_prog_eco_temp--;
						
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					conf_step = 2;

					for (uint8_t k = 0; k < 7; k++) 
					{
						heat_schedule[k][1].temperature = set_prog_eco_temp;	
						heat_schedule[k][3].temperature = set_prog_eco_temp;	
					}
					clear_LCD();
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}

			break;

			case 2:		// COMF TEMP SET

				symbol_on(ICON_CELSIUS);
				symbol_on(ICON_CLOCK);

				if (blinking == pdTRUE)
				{
					symbol_on(ICON_SUN);					
					temperature_set_large(set_prog_comfort_temp);
				}
				else if (blinking == pdFALSE)
				{
					symbol_off(ICON_SUN);					
					large_digits_blank();
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_prog_comfort_temp < 45)
					{
						set_prog_comfort_temp++;
						
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (set_prog_comfort_temp < 45)
					{
						set_prog_comfort_temp++;
						
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_prog_comfort_temp > 5)
					{
						set_prog_comfort_temp--;
						
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (set_prog_comfort_temp > 5)
					{
						set_prog_comfort_temp--;
						
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					conf_step = 3;

					for (uint8_t k = 0; k < 7; k++) 
					{
						heat_schedule[k][0].temperature = set_prog_comfort_temp;	
						heat_schedule[k][2].temperature = set_prog_comfort_temp;	
					}
					clear_LCD();
					for (uint8_t i = 0; i < 7; i++)
					{
						symbol_on(ICON_WD1 + i);
					}
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}			
			break;
			
			case 3:		// WORK OR WEEKEND SET
							

				
				if (blinking == pdTRUE)
				{
					symbol_off(ICON_SUN);
					symbol_off(ICON_WD1+day_set);
				}
				else if (blinking == pdFALSE)
				{
					if(work_weekend[day_set]) 
					{
						symbol_on(ICON_SUN);
					}
					else 
					{
						symbol_off(ICON_SUN);
					}
					symbol_on(ICON_WD1+day_set);
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					work_weekend[day_set] = !work_weekend[day_set];
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					work_weekend[day_set] = !work_weekend[day_set];
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					day_set++;
					for (uint8_t i = 0; i < 7; i++)
					{
						symbol_on(ICON_WD1 + i);
					}
					if (day_set == 7)
					{
						day_set = 0;
						conf_step = 4;
						clear_LCD();
					}
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
			break;

 			case 4:		// WORK DAY MORNING INTERVAL

				for (uint8_t i = 0; i < 7; i++)
				{
					if (!work_weekend[i]) symbol_on(ICON_WD1 + i);
				}

				if(event_time)
				{
					temperature_set_large(set_prog_eco_temp);
				}
				else
				{
					temperature_set_large(set_prog_comfort_temp);
				}
				
				symbol_on(ICON_CELSIUS);
				symbol_on(ICON_CLOCK);

				if (blinking == pdTRUE)
				{
					event_time_small(event_time, 0); 
					symbol_on(ICON_WAKE + event_time);
				}
				else
				{
					small_digit_blank(2);
					small_digit_blank(3);
					small_digit_blank(4);
					small_digit_blank(5);
					symbol_off(ICON_WAKE + event_time);					
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (!event_time)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if ((heat_schedule[k][0].event_time < (heat_schedule[k][1].event_time - 10)))
								{
									heat_schedule[k][0].event_time += 10;
								}
								else
								{
									if (heat_schedule[k][1].event_time < (heat_schedule[k][2].event_time - 10))
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
									}
									else if (heat_schedule[0][2].event_time < (heat_schedule[k][3].event_time - 10))
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
										heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
									}
									else if (heat_schedule[0][3].event_time < 1430)
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
										heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
										heat_schedule[k][3].event_time += 10;
									}
								}
							}
						}
					}
					else
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][1].event_time < (heat_schedule[k][2].event_time))
								{
									heat_schedule[k][1].event_time += 10;
								}
								else if (heat_schedule[k][2].event_time < (heat_schedule[k][3].event_time - 10))
								{
									heat_schedule[k][1].event_time += 10;
									heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
								}
								else if (heat_schedule[k][3].event_time < 1430)
								{
									heat_schedule[k][1].event_time += 10;
									heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
									heat_schedule[k][3].event_time += 10;
								}
							}
						}
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (!event_time)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if ((heat_schedule[k][0].event_time < (heat_schedule[k][1].event_time - 10)))
								{
									heat_schedule[k][0].event_time += 10;
								}
								else
								{
									if (heat_schedule[k][1].event_time < (heat_schedule[k][2].event_time - 10))
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
									}
									else if (heat_schedule[0][2].event_time < (heat_schedule[k][3].event_time - 10))
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
										heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
									}
									else if (heat_schedule[0][3].event_time < 1430)
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
										heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
										heat_schedule[k][3].event_time += 10;
									}
								}
							}
						}
					}
					else
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][1].event_time < (heat_schedule[k][2].event_time))
								{
									heat_schedule[k][1].event_time += 10;
								}
								else if (heat_schedule[k][2].event_time < (heat_schedule[k][3].event_time - 10))
								{
									heat_schedule[k][1].event_time += 10;
									heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
								}
								else if (heat_schedule[k][3].event_time < 1430)
								{
									heat_schedule[k][1].event_time += 10;
									heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
									heat_schedule[k][3].event_time += 10;
								}
							}
						}
					}
				}


				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (!event_time)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][0].event_time > 0)
								{
									heat_schedule[k][0].event_time -= 10;
								}
							}
						}
					}
					else
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
								{
									heat_schedule[k][1].event_time -= 10;
								}
								else
								{
									if (heat_schedule[k][0].event_time > 0)
									{
										heat_schedule[k][0].event_time -= 10;
										heat_schedule[k][1].event_time -= 10;
									}
								}
							}
						}
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (!event_time)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][0].event_time > 0)
								{
									heat_schedule[k][0].event_time -= 10;
								}
							}
						}
					}
					else
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
								{
									heat_schedule[k][1].event_time -= 10;
								}
								else
								{
									if ((heat_schedule[k][0].event_time > 0) && (!work_weekend[k]))
									{
										heat_schedule[k][0].event_time -= 10;
										heat_schedule[k][1].event_time -= 10;
									}
								}
							}
						}
					}
				}


				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					event_time++;
					clear_LCD();
					if(event_time == 2) 
					{
						conf_step = 5;
					}
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
			break;

			case 5:		// WORK DAY EVENING INTERVAL

				for (uint8_t i = 0; i < 7; i++)
				{
					if (!work_weekend[i]) symbol_on(ICON_WD1 + i);
				}

				if(event_time == 2)
				{
					temperature_set_large(set_prog_comfort_temp);
				}
				else 
				{
					temperature_set_large(set_prog_eco_temp);
				}
				
				symbol_on(ICON_CELSIUS);
				symbol_on(ICON_CLOCK);

				if (blinking == pdTRUE)
				{
					event_time_small(event_time, 0); 
					symbol_on(ICON_WAKE + event_time);
				}
				else
				{
					small_digit_blank(2);
					small_digit_blank(3);
					small_digit_blank(4);
					small_digit_blank(5);
					symbol_off(ICON_WAKE + event_time);					
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if ((heat_schedule[k][2].event_time < (heat_schedule[k][3].event_time - 10)))
								{
									heat_schedule[k][2].event_time += 10;
								}
								else
								{
									if (heat_schedule[k][3].event_time < 1430)
									{
										heat_schedule[k][2].event_time += 10;
										heat_schedule[k][3].event_time += 10;
									}
								}
							}
						}
					}
					else if(event_time == 3)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][3].event_time < 1430)
								{
									heat_schedule[k][3].event_time += 10;
								}
							}
						}
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if(event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if ((heat_schedule[k][2].event_time < (heat_schedule[k][3].event_time - 10)))
								{
									heat_schedule[k][2].event_time += 10;
								}
								else
								{
									if (heat_schedule[k][3].event_time < 1430)
									{
										heat_schedule[k][2].event_time += 10;
										heat_schedule[k][3].event_time += 10;
									}
								}
							}
						}
					}
					else if(event_time == 3)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][3].event_time < 1430)
								{
									heat_schedule[k][3].event_time += 10;
								}
							}
						}
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if(event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][2].event_time > (heat_schedule[k][1].event_time + 10))
								{
									heat_schedule[k][2].event_time -= 10;
								}
								else
								{
									if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
									{
										heat_schedule[k][2].event_time -= 10;
										heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
									}
									else if (heat_schedule[k][0].event_time > 0)
									{
										heat_schedule[k][2].event_time -= 10;
										heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
										heat_schedule[k][0].event_time -= 10;
									}
								}
							}
						}
					}
					else if(event_time == 3)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][3].event_time > (heat_schedule[k][2].event_time + 10))
									heat_schedule[k][3].event_time -= 10;
								else if (heat_schedule[0][2].event_time > (heat_schedule[k][1].event_time + 10))
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
								}
								else if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
									heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
								}
								else if (heat_schedule[k][0].event_time > 0)
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
									heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
									heat_schedule[k][0].event_time -= 10;
								}
							}
						}
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if(event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][2].event_time > (heat_schedule[k][1].event_time + 10))
								{
									heat_schedule[k][2].event_time -= 10;
								}
								else
								{
									if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
									{
										heat_schedule[k][2].event_time -= 10;
										heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
									}
									else if (heat_schedule[k][0].event_time > 0)
									{
										heat_schedule[k][2].event_time -= 10;
										heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
										heat_schedule[k][0].event_time -= 10;
									}
								}
							}
						}
					}
					else if(event_time == 3)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (!work_weekend[k])
							{
								if (heat_schedule[k][3].event_time > (heat_schedule[k][2].event_time + 10))
									heat_schedule[k][3].event_time -= 10;
								else if (heat_schedule[0][2].event_time > (heat_schedule[k][1].event_time + 10))
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
								}
								else if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
									heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
								}
								else if (heat_schedule[k][0].event_time > 0)
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
									heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
									heat_schedule[k][0].event_time -= 10;
								}
							}
						}
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					event_time++;
					clear_LCD();
					if(event_time == 4) 
					{
						uint8_t w_day = 0;
						for (uint8_t k = 0; k < 7; k++)
						{
							w_day += work_weekend[k];
						}
						if(w_day)
						{
							conf_step = 6;
						}
						else
						{
							conf_step = 8;
						}
						event_time = 0;
						clear_LCD();
					}
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
			break;

			case 6:		// WEEKEND MORNING INTERVAL

				for (uint8_t i = 0; i < 7; i++)
				{
					if (work_weekend[i])
						symbol_on(ICON_WD1 + i);
				}

				if(event_time)
				{
					temperature_set_large(set_prog_eco_temp);
				}
				else
				{
					temperature_set_large(set_prog_comfort_temp);
				}
				
				symbol_on(ICON_CELSIUS);
				symbol_on(ICON_CLOCK);

				if (blinking == pdTRUE)
				{
					event_time_small(event_time, 1); 
					symbol_on(ICON_WAKE + event_time);
				}
				else
				{
					small_digit_blank(2);
					small_digit_blank(3);
					small_digit_blank(4);
					small_digit_blank(5);
					symbol_off(ICON_WAKE + event_time);					
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (!event_time)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if ((heat_schedule[k][0].event_time < (heat_schedule[k][1].event_time - 10)))
								{
									heat_schedule[k][0].event_time += 10;
								}
								else
								{
									if (heat_schedule[k][1].event_time < (heat_schedule[k][2].event_time - 10))
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
									}
									else if (heat_schedule[0][2].event_time < (heat_schedule[k][3].event_time - 10))
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
										heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
									}
									else if (heat_schedule[0][3].event_time < 1430)
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
										heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
										heat_schedule[k][3].event_time += 10;
									}
								}
							}
						}
					}
					else
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][1].event_time < (heat_schedule[k][2].event_time))
								{
									heat_schedule[k][1].event_time += 10;
								}
								else if (heat_schedule[k][2].event_time < (heat_schedule[k][3].event_time - 10))
								{
									heat_schedule[k][1].event_time += 10;
									heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
								}
								else if (heat_schedule[k][3].event_time < 1430)
								{
									heat_schedule[k][1].event_time += 10;
									heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
									heat_schedule[k][3].event_time += 10;
								}
							}
						}
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (!event_time)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if ((heat_schedule[k][0].event_time < (heat_schedule[k][1].event_time - 10)))
								{
									heat_schedule[k][0].event_time += 10;
								}
								else
								{
									if (heat_schedule[k][1].event_time < (heat_schedule[k][2].event_time - 10))
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
									}
									else if (heat_schedule[0][2].event_time < (heat_schedule[k][3].event_time - 10))
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
										heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
									}
									else if (heat_schedule[0][3].event_time < 1430)
									{
										heat_schedule[k][0].event_time += 10;
										heat_schedule[k][1].event_time += 10;
										heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
										heat_schedule[k][3].event_time += 10;
									}
								}
							}
						}
					}
					else
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][1].event_time < (heat_schedule[k][2].event_time))
								{
									heat_schedule[k][1].event_time += 10;
								}
								else if (heat_schedule[k][2].event_time < (heat_schedule[k][3].event_time - 10))
								{
									heat_schedule[k][1].event_time += 10;
									heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
								}
								else if (heat_schedule[k][3].event_time < 1430)
								{
									heat_schedule[k][1].event_time += 10;
									heat_schedule[k][2].event_time = heat_schedule[k][1].event_time;
									heat_schedule[k][3].event_time += 10;
								}
							}
						}
					}
				}


				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (!event_time)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][0].event_time > 0)
								{
									heat_schedule[k][0].event_time -= 10;
								}
							}
						}
					}
					else
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
								{
									heat_schedule[k][1].event_time -= 10;
								}
								else
								{
									if (heat_schedule[k][0].event_time > 0)
									{
										heat_schedule[k][0].event_time -= 10;
										heat_schedule[k][1].event_time -= 10;
									}
								}
							}
						}
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (!event_time)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][0].event_time > 0)
								{
									heat_schedule[k][0].event_time -= 10;
								}
							}
						}
					}
					else
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
								{
									heat_schedule[k][1].event_time -= 10;
								}
								else
								{
									if ((heat_schedule[k][0].event_time > 0) && (!work_weekend[k]))
									{
										heat_schedule[k][0].event_time -= 10;
										heat_schedule[k][1].event_time -= 10;
									}
								}
							}
						}
					}
				}


				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					event_time++;
					clear_LCD();
					if(event_time == 2) 
					{
						conf_step = 7;
						clear_LCD();
					}
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}			
			break;

			case 7:		// WEEKEND EVENING INTERVAL

				for (uint8_t i = 0; i < 7; i++)
				{
					if (work_weekend[i])
						symbol_on(ICON_WD1 + i);
				}

				if(event_time == 2)
				{
					temperature_set_large(set_prog_comfort_temp);
				}
				else 
				{
					temperature_set_large(set_prog_eco_temp);
				}
				
				symbol_on(ICON_CELSIUS);
				symbol_on(ICON_CLOCK);

				if (blinking == pdTRUE)
				{
					event_time_small(event_time, 1); 
					symbol_on(ICON_WAKE + event_time);
				}
				else
				{
					small_digit_blank(2);
					small_digit_blank(3);
					small_digit_blank(4);
					small_digit_blank(5);
					symbol_off(ICON_WAKE + event_time);					
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if ((heat_schedule[k][2].event_time < (heat_schedule[k][3].event_time - 10)))
								{
									heat_schedule[k][2].event_time += 10;
								}
								else
								{
									if (heat_schedule[k][3].event_time < 1430)
									{
										heat_schedule[k][2].event_time += 10;
										heat_schedule[k][3].event_time += 10;
									}
								}
							}
						}
					}
					else if (event_time == 3)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][3].event_time < 1430)
								{
									heat_schedule[k][3].event_time += 10;
								}
							}
						}
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if ((heat_schedule[k][2].event_time < (heat_schedule[k][3].event_time - 10)))
								{
									heat_schedule[k][2].event_time += 10;
								}
								else
								{
									if (heat_schedule[k][3].event_time < 1430)
									{
										heat_schedule[k][2].event_time += 10;
										heat_schedule[k][3].event_time += 10;
									}
								}
							}
						}
					}
					else if (event_time == 3)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][3].event_time < 1430)
								{
									heat_schedule[k][3].event_time += 10;
								}
							}
						}
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][2].event_time > (heat_schedule[k][1].event_time + 10))
								{
									heat_schedule[k][2].event_time -= 10;
								}
								else
								{
									if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
									{
										heat_schedule[k][2].event_time -= 10;
										heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
									}
									else if (heat_schedule[k][0].event_time > 0)
									{
										heat_schedule[k][2].event_time -= 10;
										heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
										heat_schedule[k][0].event_time -= 10;
									}
								}
							}
						}
					}
					else if (event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][3].event_time > (heat_schedule[k][2].event_time + 10))
									heat_schedule[k][3].event_time -= 10;
								else if (heat_schedule[0][2].event_time > (heat_schedule[k][1].event_time + 10))
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
								}
								else if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
									heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
								}
								else if (heat_schedule[k][0].event_time > 0)
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
									heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
									heat_schedule[k][0].event_time -= 10;
								}
							}
						}
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (event_time == 2)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][2].event_time > (heat_schedule[k][1].event_time + 10))
								{
									heat_schedule[k][2].event_time -= 10;
								}
								else
								{
									if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
									{
										heat_schedule[k][2].event_time -= 10;
										heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
									}
									else if (heat_schedule[k][0].event_time > 0)
									{
										heat_schedule[k][2].event_time -= 10;
										heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
										heat_schedule[k][0].event_time -= 10;
									}
								}
							}
						}
					}
					else if (event_time == 3)
					{
						for (uint8_t k = 0; k < 7; k++)
						{
							if (work_weekend[k])
							{
								if (heat_schedule[k][3].event_time > (heat_schedule[k][2].event_time + 10))
									heat_schedule[k][3].event_time -= 10;
								else if (heat_schedule[0][2].event_time > (heat_schedule[k][1].event_time + 10))
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
								}
								else if (heat_schedule[k][1].event_time > (heat_schedule[k][0].event_time + 10))
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
									heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
								}
								else if (heat_schedule[k][0].event_time > 0)
								{
									heat_schedule[k][3].event_time -= 10;
									heat_schedule[k][2].event_time -= 10;
									heat_schedule[k][1].event_time = heat_schedule[k][2].event_time;
									heat_schedule[k][0].event_time -= 10;
								}
							}
						}
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					event_time++;
					clear_LCD();
					if (event_time == 4)
					{
						conf_step = 8;
						event_time = 0;
						clear_LCD();
						for (uint8_t i = 0; i < 7; i++)
						{
							for (uint8_t j = 0; j < 4; j++)
							{
								printf("Time_prog %d Temp_set %d // ",heat_schedule[i][j].event_time, heat_schedule[i][j].temperature);
							}
							printf("\n");
						} 
					}
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
			break;

			case 8:		// TIME SET 

				symbol_on(ICON_CLOCK);  

				if (blinking == pdTRUE)
				{
					date_time_small();
				}
				else if (blinking == pdFALSE)
				{
					small_digit_blank(2 + (event_time * 2));
					small_digit_blank(3 + (event_time * 2));
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					if (!event_time)
					{ // hours
						if (RTC_TimeStr.RTC_Hours < 23)
						{
							RTC_TimeStr.RTC_Hours++;
						}
						else
						{
							RTC_TimeStr.RTC_Hours = 0;
						}
						small_digit(2, RTC_TimeStr.RTC_Hours / 10);
						small_digit(3, RTC_TimeStr.RTC_Hours % 10);
					}
					else
					{ // minutes
						if (RTC_TimeStr.RTC_Minutes < 59)
						{
							RTC_TimeStr.RTC_Minutes++;
						}
						else
						{
							RTC_TimeStr.RTC_Minutes = 0;
						}
						small_digit(4, RTC_TimeStr.RTC_Minutes / 10);
						small_digit(5, RTC_TimeStr.RTC_Minutes % 10);
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (!event_time)
					{ // hours
						if (RTC_TimeStr.RTC_Hours < 23)
						{
							RTC_TimeStr.RTC_Hours++;
						}
						else
						{
							RTC_TimeStr.RTC_Hours = 0;
						}
						small_digit(2, RTC_TimeStr.RTC_Hours / 10);
						small_digit(3, RTC_TimeStr.RTC_Hours % 10);
					}
					else
					{ // minutes
						if (RTC_TimeStr.RTC_Minutes < 59)
						{
							RTC_TimeStr.RTC_Minutes++;
						}
						else
						{
							RTC_TimeStr.RTC_Minutes = 0;
						}
						small_digit(4, RTC_TimeStr.RTC_Minutes / 10);
						small_digit(5, RTC_TimeStr.RTC_Minutes % 10);
					}

				}				

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					if (!event_time)
					{ 
						if (RTC_TimeStr.RTC_Hours > 0)
						{
							RTC_TimeStr.RTC_Hours--;
						}
						else
						{
							RTC_TimeStr.RTC_Hours = 23;
						}
						small_digit(2, RTC_TimeStr.RTC_Hours / 10);
						small_digit(3, RTC_TimeStr.RTC_Hours % 10);
					}
					else
					{ 
						if (RTC_TimeStr.RTC_Minutes > 0)
						{
							RTC_TimeStr.RTC_Minutes--;
						}
						else
						{
							RTC_TimeStr.RTC_Minutes = 59;
						}
						small_digit(4, RTC_TimeStr.RTC_Minutes / 10);
						small_digit(5, RTC_TimeStr.RTC_Minutes % 10);
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (!event_time)
					{ 
						if (RTC_TimeStr.RTC_Hours > 0)
						{
							RTC_TimeStr.RTC_Hours--;
						}
						else
						{
							RTC_TimeStr.RTC_Hours = 23;
						}
						small_digit(2, RTC_TimeStr.RTC_Hours / 10);
						small_digit(3, RTC_TimeStr.RTC_Hours % 10);
					}
					else
					{ 
						if (RTC_TimeStr.RTC_Minutes > 0)
						{
							RTC_TimeStr.RTC_Minutes--;
						}
						else
						{
							RTC_TimeStr.RTC_Minutes = 59;
						}
						small_digit(4, RTC_TimeStr.RTC_Minutes / 10);
						small_digit(5, RTC_TimeStr.RTC_Minutes % 10);
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					event_time++;
					if(event_time == 2) 
					{
						conf_step = 9;
						clear_LCD();
					}
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
			break;

			case 9:		// DATE SET

				symbol_on(ICON_CLOCK);  

				if (blinking == pdTRUE)
				{
					date_time_small();
				}
				else if (blinking == pdFALSE)
				{
					small_digit_blank(2 + (event_time * 2));
					small_digit_blank(3 + (event_time * 2));
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					if (event_time == 2)
					{ // date
						if (RTC_DateStr.RTC_Date < 31)
						{
							RTC_DateStr.RTC_Date++;
						}
						else
						{
							RTC_DateStr.RTC_Date = 0;
						}
						small_digit(6, RTC_DateStr.RTC_Date / 10);
						small_digit(7, RTC_DateStr.RTC_Date % 10);
					}
					else
					{ // month
						if (RTC_DateStr.RTC_Month < 12)
						{
							RTC_DateStr.RTC_Month++;
						}
						else
						{
							RTC_DateStr.RTC_Month = 0;
						}
						small_digit(8, RTC_DateStr.RTC_Month / 10);
						small_digit(9, RTC_DateStr.RTC_Month % 10);
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (!event_time)
					{ // date
						if (RTC_DateStr.RTC_Date < 31)
						{
							RTC_DateStr.RTC_Date++;
						}
						else
						{
							RTC_DateStr.RTC_Date = 0;
						}
						small_digit(6, RTC_DateStr.RTC_Date / 10);
						small_digit(7, RTC_DateStr.RTC_Date % 10);
					}
					else
					{ // month
						if (RTC_DateStr.RTC_Month < 12)
						{
							RTC_DateStr.RTC_Month++;
						}
						else
						{
							RTC_DateStr.RTC_Month = 0;
						}
						small_digit(8, RTC_DateStr.RTC_Month / 10);
						small_digit(9, RTC_DateStr.RTC_Month % 10);
					}

				}				

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					if (!event_time)
					{ 
						if (RTC_DateStr.RTC_Date > 0)
						{
							RTC_DateStr.RTC_Date--;
						}
						else
						{
							RTC_DateStr.RTC_Date = 31;
						}
						small_digit(6, RTC_DateStr.RTC_Date / 10);
						small_digit(7, RTC_DateStr.RTC_Date % 10);
					}
					else
					{ 
						if (RTC_DateStr.RTC_Month > 0)
						{
							RTC_DateStr.RTC_Month--;
						}
						else
						{
							RTC_DateStr.RTC_Month = 12;
						}
						small_digit(8, RTC_DateStr.RTC_Month / 10);
						small_digit(9, RTC_DateStr.RTC_Month % 10);
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (!event_time)
					{ 
						if (RTC_DateStr.RTC_Date > 0)
						{
							RTC_DateStr.RTC_Date--;
						}
						else
						{
							RTC_DateStr.RTC_Date = 31;
						}
						small_digit(6, RTC_DateStr.RTC_Date / 10);
						small_digit(7, RTC_DateStr.RTC_Date % 10);
					}
					else
					{ 
						if (RTC_DateStr.RTC_Month > 0)
						{
							RTC_DateStr.RTC_Month--;
						}
						else
						{
							RTC_DateStr.RTC_Month = 12;
						}
						small_digit(8, RTC_DateStr.RTC_Month / 10);
						small_digit(9, RTC_DateStr.RTC_Month % 10);
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					event_time++;
					if(event_time == 4) 
					{
						conf_step = 10;
						event_time = 0;
						clear_LCD();
						full_year = 2000 + RTC_DateStr.RTC_Year;
					}				
				}
				
				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
								
			break;

			case 10:	// YEAR SET

				symbol_on(ICON_CLOCK);  

				if (blinking == pdTRUE)
				{
					small_digit(6, (full_year / 1000));
					small_digit(7, (full_year / 100) % 10);
					small_digit(8, (full_year % 100) / 10);
					small_digit(9, (full_year % 100) % 10);					
				}
				else if (blinking == pdFALSE)
				{
					small_digit_blank(6);
					small_digit_blank(7);
					small_digit_blank(8);
					small_digit_blank(9);
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (full_year < 2099)
					{
						full_year++;
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;
					if (full_year < 2099)
					{
						full_year++;
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (full_year > 2000)
					{
						full_year--;
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (full_year > 2000)
					{
						full_year--;
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					conf_step = 11;
					RTC_DateStr.RTC_Year = full_year - 2000;
					clear_LCD();
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
			break;

			case 11:	// SENSOR TYPE SET

				temperature_set_large(floor_temperature);				
				
 				if (blinking == pdTRUE)
				{
					small_digit_blank(6);
					small_digit_blank(7);
					small_digit_blank(8);
					symbol_off(ICON_DOT);
					symbol_off(ICON_FLOR);
				}
				else
				{
					sens_type_to_LCD(sensor_type);
					symbol_on(ICON_FLOR);
				} 

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					small_digit_blank(6);
					small_digit_blank(7);
					small_digit_blank(8);

					if(sensor_type < 6)
					{
						sensor_type++;
					}
					else
					{
						sensor_type = 1;
					}

					uint16_t adc_floor = 0;
					for (int adc_1 = 0; adc_1 < 10; adc_1++)
					{
						adc_floor += adc1_get_raw(ADC1_CHANNEL_6);
					}
					adc_floor /= 10;
					uint16_t voltage1 = esp_adc_cal_raw_to_voltage(adc_floor, adc_chars);
					floor_temperature = Calculate_Temper(voltage1, sensor_type - 1);
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					small_digit_blank(6);
					small_digit_blank(7);
					small_digit_blank(8);

					if(sensor_type > 1)
					{
						sensor_type--;
					}
					else
					{
						sensor_type = 6;
					}

					uint16_t adc_floor = 0;
					for (int adc_1 = 0; adc_1 < 10; adc_1++)
					{
						adc_floor += adc1_get_raw(ADC1_CHANNEL_6);
					}
					adc_floor /= 10;
					uint16_t voltage1 = esp_adc_cal_raw_to_voltage(adc_floor, adc_chars);
					floor_temperature = Calculate_Temper(voltage1, sensor_type - 1);					
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					conf_step = 12;
					
					clear_LCD();
				}				

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
			break;

			case 12:	// SENSORS & SELFTRAINING SET

				if (blinking == pdTRUE)
				{
					if (sensor_set & 1)
					{
						symbol_on(ICON_AIR);
					}
					else
					{
						symbol_off(ICON_AIR);
					}
					if (sensor_set & 2)
					{
						symbol_on(ICON_FLOR);
					}
					else
					{
						symbol_off(ICON_FLOR);
						sensor_err = 0;
					}
					if (sensor_set & 4)
					{
						symbol_on(ICON_HAT);
					}
					else
					{
						symbol_off(ICON_HAT);
					}
				}
				else
				{
					symbol_off(ICON_FLOR); 
					symbol_off(ICON_AIR);  
					symbol_off(ICON_HAT);
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					if (sensor_set & 4)
						sensor_set &= ~0x04;
					else
						sensor_set |= 0x04;
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					if ((sensor_set & 3) < 3)
						sensor_set++;
					else
					{
						sensor_set &= 0x3C;
						sensor_set |= 0x01;
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					conf_step = 13;
					
					clear_LCD();
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}
			break;

			case 13:	// AIR TEMP SET

				if (blinking == pdTRUE)
				{
					symbol_on(ICON_AIR);
					temperature_set_large(set_air_temperature);
				}
				else
				{
					symbol_off(ICON_AIR);
					large_digits_blank();
				}

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_air_temperature < 45)
					{
						set_air_temperature++;
					}
				}

				if ((BT_PLUS_PAR.type_click == DSHORT_CLICK) && (BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (set_air_temperature < 45)
					{
						set_air_temperature++;
					}
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if (set_air_temperature > 5)
					{
						set_air_temperature--;
					}
				}

				if ((BT_MINUS_PAR.type_click == DSHORT_CLICK) && (BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 30;
					bkl_led_off_timer = BKL_OFF_TIME;

					if (set_air_temperature > 5)
					{
						set_air_temperature--;
					}
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					conf_step = 14;
					clear_LCD();
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}		
			break;

			case 14:	// CORRECTION OF AIR TEMP

				symbol_on(ICON_AIR);
				
				small_digit(2, 12);
				small_digit(3, 13);
				small_digit(4, 14);
				small_digit(5, 14);

				if (blinking == pdTRUE)
				{
					if (delta_air_temperature >= 5)
					{
						small_digit(9,(delta_air_temperature - 5));
						small_digit_blank(8);
					}
					else
					{
						small_digit(9,(5 - delta_air_temperature));
						small_digit(8, 10);
					} 
				}
				else
				{
					small_digit_blank(8);
					small_digit_blank(9);
				}

 				if (delta_air_temperature >= 5)
				{
					temperature_set_large(air_temperature + (delta_air_temperature - 5));
				}
				else
				{
					temperature_set_large(air_temperature - (5 - delta_air_temperature));
				} 

				if ((BT_PLUS_PAR.type_click == SHORT_CLICK) && (!BT_PLUS_PAR.flag_click))
				{
					BT_PLUS_PAR.type_click = NO_CLICK;
					BT_PLUS_PAR.counter = 0;
					BT_PLUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if(delta_air_temperature < 10)
					delta_air_temperature++;
					else
					delta_air_temperature = 10;
				}

				if ((BT_MINUS_PAR.type_click == SHORT_CLICK) && (!BT_MINUS_PAR.flag_click))
				{
					BT_MINUS_PAR.type_click = NO_CLICK;
					BT_MINUS_PAR.counter = 0;
					BT_MINUS_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;
					if(delta_air_temperature > 0)
					delta_air_temperature--;
					else
					delta_air_temperature = 0;
				}

				if ((BT_MODE_OK_PAR.type_click == SHORT_CLICK) && (!BT_MODE_OK_PAR.flag_click))
				{
					BT_MODE_OK_PAR.type_click = NO_CLICK;
					BT_MODE_OK_PAR.counter = 0;
					BT_MODE_OK_PAR.flag_click = 0;
					beep_beep();
					bkl_led_off_timer = BKL_OFF_TIME;

					conf_step = 15;
					clear_LCD();
				}

				if ((BT_SET_CANCEL_PAR.type_click == SHORT_CLICK) && (BT_SET_CANCEL_PAR.flag_click))
				{
					BT_SET_CANCEL_PAR.type_click = NO_CLICK;
					BT_SET_CANCEL_PAR.counter=0;
					BT_SET_CANCEL_PAR.flag_click=0;
					beep_beep();
					conf_step = 15;
					clear_LCD();
				}					
			break;

			case 15: 	// SAVE ALL SETTINGS TO FLASH

				nvs_open("storage", NVS_READWRITE, &my_handle);

				set_prog_comfort_temp = heat_schedule[0][0].temperature;
				set_prog_eco_temp     = heat_schedule[0][1].temperature;

				size_t heat_schedule_len = 112; 
				size_t work_weekend_len  = 7;
				nvs_set_blob(my_handle, "heat_schedule", (const void *)heat_schedule, heat_schedule_len);
				nvs_set_blob(my_handle,  "work_weekend", (const void *)work_weekend, work_weekend_len);

				rtc_set(BINtoBCD(0), BINtoBCD(RTC_TimeStr.RTC_Minutes),  BINtoBCD(RTC_TimeStr.RTC_Hours),  BINtoBCD(RTC_DateStr.RTC_Date),  RTC_DateStr.RTC_WeekDay,  BINtoBCD(RTC_DateStr.RTC_Month),  BINtoBCD(RTC_DateStr.RTC_Year));

				nvs_set_u8(my_handle, "sensor_type",    sensor_type);
				nvs_set_u8(my_handle, "sensor_set",     sensor_set);
				nvs_set_u8(my_handle, "set_air_temp",   set_air_temperature);
				nvs_set_u8(my_handle, "delta_air_temp", delta_air_temperature);

				nvs_commit(my_handle);
				nvs_close(my_handle);
				if(bkl_led_off_timer == 0)
				{
					device_status = DEVICE_SCREEN_LOCK;
				}
				else
				{
					device_status = DEVICE_SCREEN_UNLOCK;
				}
				conf_step = 1;
				lcd_redraw();

			break;
			}

			break;
		}


 	if (!plus_minus_flag)
	{ 
		//printf("plus_minus_flag 0\n");
		if ((BT_MODE_OK_PAR.type_click <= DLONG_CLICK) && (!BT_MODE_OK_PAR.flag_click))
		{
			BT_MODE_OK_PAR.type_click = NO_CLICK;
		}
		if ((BT_MINUS_PAR.type_click <= DLONG_CLICK) && (!BT_MINUS_PAR.flag_click))
		{
			BT_MINUS_PAR.type_click = NO_CLICK;
		}
		if ((BT_PLUS_PAR.type_click <= DLONG_CLICK) && (!BT_PLUS_PAR.flag_click))
		{
			BT_PLUS_PAR.type_click = NO_CLICK;
		}
		if ((BT_ON_OFF_PAR.type_click <= DLONG_CLICK) && (!BT_ON_OFF_PAR.flag_click))
		{
			BT_ON_OFF_PAR.type_click = NO_CLICK;
		}
		if ((BT_SET_CANCEL_PAR.type_click <= DLONG_CLICK) && (!BT_SET_CANCEL_PAR.flag_click))
		{
			BT_SET_CANCEL_PAR.type_click = NO_CLICK;
		}
 	}
	else if (plus_minus_flag && (!BT_MODE_OK_PAR.flag_click) && (!BT_MINUS_PAR.flag_click) && (!BT_PLUS_PAR.flag_click)  && (!BT_ON_OFF_PAR.flag_click)  && (!BT_SET_CANCEL_PAR.flag_click))
	{
		//printf("plus_minus_flag 1\n");
		if ((BT_MODE_OK_PAR.type_click <= DLONG_CLICK) && (!BT_MODE_OK_PAR.flag_click))
		{
			BT_MODE_OK_PAR.type_click = NO_CLICK;
		}
		if ((BT_MINUS_PAR.type_click <= DLONG_CLICK) && (!BT_MINUS_PAR.flag_click))
		{
			BT_MINUS_PAR.type_click = NO_CLICK;
		}
		if ((BT_PLUS_PAR.type_click <= DLONG_CLICK) && (!BT_PLUS_PAR.flag_click))
		{
			BT_PLUS_PAR.type_click = NO_CLICK;
		}
		if ((BT_ON_OFF_PAR.type_click <= DLONG_CLICK) && (!BT_ON_OFF_PAR.flag_click))
		{
			BT_ON_OFF_PAR.type_click = NO_CLICK;
		}
		if ((BT_SET_CANCEL_PAR.type_click <= DLONG_CLICK) && (!BT_SET_CANCEL_PAR.flag_click))
		{
			BT_SET_CANCEL_PAR.type_click = NO_CLICK;
		}
		
		plus_minus_flag = 0;
		
	} 
	
} 

//==========================================================================================
//================================================================ Function calc temperature
//==========================================================================================
int8_t Calculate_Temper(uint16_t Code, uint8_t Sens_type)
{
	  int8_t Temper[37]={       -55,     -50,     -45,     -40,     -35,     -30,     -25,     -20,     -15,     -10,      -5,       0,       5,      10,      15,      20,       25,      30,      35,      40,      45,      50,     55,     60,     65,     70,     75,     80,     85,     90,     95,    100,    105,    110,    115,    120,    125  };
	  float Resistor[6][37]={
							  {3143.87, 3079.56, 2995.17, 2887.57, 2754.51, 2596.14, 2413.56, 2212.25, 1998.05, 1780.00, 1565.53, 1362.58, 1175.78, 1008.96,  862.77,  737.34,  630.88,  541.57,  467.16,  405.57,  354.60,  312.50, 277.85, 249.15, 225.46, 205.81, 189.54, 175.91, 164.55, 155.02, 147.01, 140.25, 134.56, 129.73, 125.58, 122.02, 118.93},
							  {3180.70, 3132.87, 3069.93, 2989.01, 2887.59, 2763.94, 2617.45, 2450.16, 2264.55, 2066.81, 1857.71, 1650.34, 1454.58, 1271.72, 1105.23,  956.75,     825,  712.07,  615.68,  534.07,  465.79,  408.41, 359.87, 319.36, 285.74, 257.60, 233.97, 214.14, 197.45, 183.38, 171.49, 161.42, 152.96, 145.75, 139.56, 134.26, 129.74},
							  {3164.15, 3114.84, 3051.85, 2973.09, 2876.81, 2761.94, 2628.39, 2477.34, 2307.72, 2126.72, 1936.22, 1745.00, 1559.30, 1382.74, 1218.11, 1068.25,  932.60,  813.78,  710.83,  621.90,  545.22,  479.85, 423.77, 376.36, 336.64, 302.98, 274.39, 250.13, 229.46, 211.86, 196.97, 184.24, 173.27, 163.84, 155.74, 148.75, 142.72},
							  {3228.69, 3197.52, 3155.46, 3099.92, 3028.23, 2937.97, 2827.02, 2694.58, 2542.38, 2371.74, 2186.46, 1991.95, 1793.10, 1597.36, 1409.60, 1234.84, 1077.55,  933.22,  808.57,  700.59,  610.41,  532.96, 464.86, 407.66, 360.07, 320.07, 286.59, 258.45, 234.93, 215.13, 198.05, 183.63, 171.96, 162.04, 153.38, 146.00, 139.77},
							  {3273.05, 3261.38, 3245.24, 3223.19, 3193.47, 3153.96, 3102.05, 3035.92, 2951.94, 2849.36, 2724.88, 2580.51, 2418.16, 2241.05, 2055.04, 1864.34, 1674.62, 1491.28, 1315.61, 1153.94, 1009.22,  880.52, 766.86, 668.42, 583.71, 511.31, 449.89, 397.81, 354.17, 317.27, 285.82, 259.20, 236.80, 217.79, 201.54, 187.70, 175.91},
							  {3280.10, 3271.33, 3259.20, 3242.64, 3220.37, 3190.80, 3152.15, 3102.42, 3039.52, 2961.46, 2866.53, 2753.66, 2622.64, 2474.49, 2311.46, 2137.01, 1955.55, 1772.04, 1591.36, 1417.85, 1254.99, 1105.21, 969.84, 849.32, 743.35, 651.08, 571.34, 502.89, 444.34, 394.45, 352.02, 315.96, 285.32, 259.29, 237.15, 218.31, 202.24},
	  };
	float Code_ADC[37];
	int8_t Res;
	uint8_t i, kr;
	float k, m;

	kr = 0;
	for(i = 0; i < 37; i++)
	{
		Code_ADC[i] = Resistor[Sens_type][i];
		if(Code >= Code_ADC[i])
		{
			kr = i;
			break;
		}
	}
	k = (float) (Temper[kr] - Temper[kr - 1]) / (Code_ADC[kr] - Code_ADC[kr - 1]);
	m = (float) ((float) Temper[kr] - k * (float) Code_ADC[kr]);
	if(((k * (float) Code + m) - ((int8_t) (k * (float) Code + m))) >= 0.5)
	{
		Res = (int8_t) (k * (float) Code + m) + 1;
	}
	else
	{
		Res = (int8_t) (k * (float) Code + m);
	}

	if(Res <= 0)
	{
		Res = 0;
	}
	if(Res > 70)
	{
		Res = 70;
	}

	return Res;
}

uint8_t array_to_byte()
{
	uint8_t w_byte = 0;
	uint8_t bw = 6;
 	for(uint8_t wb = 0; wb < 7; wb++)
	{
		//w_byte |= (work_weekend[wb] << bw);
		w_byte |= (work_weekend[wb] << wb);		
		bw--;
	}
	//printf("%d\n", w_byte);
	return w_byte; 
}

void byte_to_array()
{
	for(uint8_t wb = 0; wb < 7; wb++)
	{
		work_weekend[wb] = ((week_byte & (1 << wb)) >> wb);
		printf("%d ",work_weekend[wb]);
	}
	printf("\n");
}