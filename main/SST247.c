#include "device_init.h"

static const char *TAG = "MCS400_WIFI";


//============================================================================================
//=============================================================== Read temp. & relay routine =
//============================================================================================
void read_temperature(void *pvParameters)
{
	while(1)
	{
  		uint16_t voltage3 = 0;
		while (!((voltage3) == 631))
		{
			uint16_t adc_cal = 0;
			for (int adc_3 = 0; adc_3 < 10; adc_3++)
			{
				adc_cal += adc1_get_raw(ADC1_CHANNEL_4);
			}
			adc_cal /= 10;

			if ((adc_cal > 500) && (adc_cal < 700))
			{
				voltage3 = esp_adc_cal_raw_to_voltage((uint32_t)adc_cal, adc_chars);
				if ((voltage3) < 631)
					calib_vref++;
				else
					calib_vref--;
				esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, calib_vref, adc_chars);
			}
			else
				break;
		}
		
		//=============================================================================================== FLOOR
		if (sensor_set & 2)
		{
			uint16_t adc_floor = 0;
			for (int adc_1 = 0; adc_1 < 10; adc_1++)
			{
				adc_floor += adc1_get_raw(ADC1_CHANNEL_6);
			}
			adc_floor /= 10;
			uint16_t voltage1 = esp_adc_cal_raw_to_voltage(adc_floor, adc_chars);
			floor_temperature = Calculate_Temper(voltage1, sensor_type - 1);
			if ((adc_floor >= 3500) || (adc_floor <= 20))
			{
				sensor_err = 1;
			}
			else
			{
				sensor_err = 0;
			}
		}
		//=============================================================================================== AIR
		uint16_t adc_air = 0;
		for (int adc_2 = 0; adc_2 < 10; adc_2++)
		{
			adc_air += adc1_get_raw(ADC1_CHANNEL_7);
		}
		adc_air /= 10;
		uint16_t voltage2 = esp_adc_cal_raw_to_voltage(adc_air, adc_chars);
		air_temperature = Calculate_Temper(voltage2, 0);

		
		//=============================================================================================== HEATING ROUTINE

		if(heat_mode == 0)
		{
			if(delta_air_temperature > 5)
			{
				if((!sensor_err) && (!relay_flag) && ((((sensor_set&3)==3)&&((air_temperature+(delta_air_temperature-5))   < set_air_temperature) && (floor_temperature < set_manual_floor_temperature))||
					                                  (((sensor_set&3)==2)&&(floor_temperature < set_manual_floor_temperature))||
					                                  (((sensor_set&3)==1)&&((air_temperature+(delta_air_temperature-5))   < set_manual_floor_temperature))))
				{
					relay_flag = 1;
					RELAY_ON;
					symbol_on(ICON_HEAT);
				}

				if(((relay_flag) &&                  ((((sensor_set&3)==3) && (((air_temperature+(delta_air_temperature-5))  > set_air_temperature) || (floor_temperature > set_manual_floor_temperature))) ||
				                     	 	 	 	  (((sensor_set&2)==2) && (floor_temperature > set_manual_floor_temperature))||
													  (((sensor_set&3)==1) && ((air_temperature+(delta_air_temperature-5))   > set_manual_floor_temperature)))))
					{
					relay_flag = 0;
					RELAY_OFF;
					symbol_off(ICON_HEAT);
				}
			}
			else
			{
				if((!sensor_err) && (!relay_flag) &&  ((((sensor_set&3)==3)&&((air_temperature-(5-delta_air_temperature))   < set_air_temperature) && (floor_temperature < set_manual_floor_temperature))||
				                                       (((sensor_set&3)==2)&&(floor_temperature < set_manual_floor_temperature))||
				                                       (((sensor_set&3)==1)&&((air_temperature-(5-delta_air_temperature))   < set_manual_floor_temperature))))
				{
					relay_flag = 1;
					RELAY_ON;
					symbol_on(ICON_HEAT);
				}

			    if(((relay_flag) &&                   ((((sensor_set&3)==3) && (((air_temperature-(5-delta_air_temperature))  > set_air_temperature) || (floor_temperature > set_manual_floor_temperature))) ||
			                     	 	 	 	       (((sensor_set&2)==2) && (floor_temperature > set_manual_floor_temperature))||
												       (((sensor_set&3)==1) && ((air_temperature-(5-delta_air_temperature))   > set_manual_floor_temperature)))))
				{
					relay_flag = 0;
					RELAY_OFF;
					symbol_off(ICON_HEAT);
				}

			}
		}

		if(heat_mode == 2)
		{
			if(delta_air_temperature > 5)
			{
				if((!sensor_err) && (!relay_flag) && ((((sensor_set&3)==3)&&((air_temperature+(delta_air_temperature-5))   < set_air_temperature) && (floor_temperature < set_antifreeze_floor_temperature))||
					                                  (((sensor_set&3)==2)&&(floor_temperature < set_antifreeze_floor_temperature))||
					                                  (((sensor_set&3)==1)&&((air_temperature+(delta_air_temperature-5))   < set_antifreeze_floor_temperature))))
				{
					relay_flag = 1;
					RELAY_ON;
					symbol_on(ICON_HEAT);
				}

				if(((relay_flag) &&                  ((((sensor_set&3)==3) && (((air_temperature+(delta_air_temperature-5))  > set_air_temperature) || (floor_temperature > set_antifreeze_floor_temperature))) ||
				                     	 	 	 	  (((sensor_set&2)==2) && (floor_temperature > set_antifreeze_floor_temperature))||
													  (((sensor_set&3)==1) && ((air_temperature+(delta_air_temperature-5))   > set_antifreeze_floor_temperature)))))
					{
					relay_flag = 0;
					RELAY_OFF;
					symbol_off(ICON_HEAT);
				}
			}
			else
			{
				if((!sensor_err) && (!relay_flag) &&  ((((sensor_set&3)==3)&&((air_temperature-(5-delta_air_temperature))   < set_air_temperature) && (floor_temperature < set_antifreeze_floor_temperature))||
				                                       (((sensor_set&3)==2)&&(floor_temperature < set_antifreeze_floor_temperature))||
				                                       (((sensor_set&3)==1)&&((air_temperature-(5-delta_air_temperature))   < set_antifreeze_floor_temperature))))
				{
					relay_flag = 1;
					RELAY_ON;
					symbol_on(ICON_HEAT);
				}

			    if(((relay_flag) &&                   ((((sensor_set&3)==3) && (((air_temperature-(5-delta_air_temperature))  > set_air_temperature) || (floor_temperature > set_antifreeze_floor_temperature))) ||
			                     	 	 	 	       (((sensor_set&2)==2) && (floor_temperature > set_antifreeze_floor_temperature))||
												       (((sensor_set&3)==1) && ((air_temperature-(5-delta_air_temperature))   > set_antifreeze_floor_temperature)))))
				{
					relay_flag = 0;
					RELAY_OFF;
					symbol_off(ICON_HEAT);
				}

			}
		}
		if(heat_mode == 1)
		{
			if(tmp_Event_now != Event_now)
			{
				tmp_Event_now = Event_now;
				flag_preheat = 0;
				set_floor_temperature_prog = heat_schedule[Day_Event_Now][Event_now].temperature;
			}

/* 			if((!flag_preheat) && ((sensor_set & 0x07) == 6))
			{

				if((heat_schedule[Day_Event_Next][Event_next].temperature > set_floor_temperature_prog))
				{

					Temp_minuts = 0;
					if(floor_temperature < heat_schedule[Day_Event_Next][Event_next].temperature)
						for(i = floor_temperature - 5;i < heat_schedule[Day_Event_Next][Event_next].temperature - 5;i++)
						{
							Temp_minuts += preheat_timetable[i]; // (10min/1C)*(amount of C for reaching ComfortTemp)
						}
					if(Temp_minuts > 120)
						Temp_minuts = 120;

					if((Event_now == 3) && ((((1440 - Time_min24 + heat_schedule[Day_Event_Next][0].event_time) <= Temp_minuts) && (Day_Event_Next != RTC_DateStr.RTC_WeekDay)) || //condition now day
					((Temp_minuts >= (heat_schedule[Day_Event_Next][0].event_time - (Time_min24))) && (Day_Event_Next == RTC_DateStr.RTC_WeekDay)))) //condition next day
					{
						flag_preheat = 1; //preHeat ON, if condition is end of evening interval and time is <= time to start morning interval
						//printf("%d %d\n",(heat_schedule[Day_Event_Next][0].event_time - Time_min24),RTC_DateStr.RTC_WeekDay);
					}
					else if((Event_now < 3) && ((Time_min24) >= (heat_schedule[Day_Event_Next][Event_next].event_time - Temp_minuts)))
					{
						flag_preheat = 1; //preHeat ON, if condition is end of morning interval and time is >= time to start evenng interval
						//printf("norm\n");

					}
				}
			} */

			if(delta_air_temperature > 5)
			{
				if ((!sensor_err) && (!relay_flag) && (((!flag_preheat) && (
						(((sensor_set & 0x03) == 3) && ((air_temperature+(delta_air_temperature - 5)) < set_air_temperature) && (floor_temperature < set_floor_temperature_prog)) ||
						(((sensor_set & 0x03) == 2) && (floor_temperature < set_floor_temperature_prog)) ||
						(((sensor_set & 0x03) == 1) && ((air_temperature+(delta_air_temperature - 5)) < set_floor_temperature_prog)))) ||

						  (flag_preheat && (sensor_set & 0x02) && (Event_now + 1 < 3) && (floor_temperature < set_floor_temperature_prog)) ||
						  (flag_preheat && (sensor_set & 0x02) && (Event_now == 3) && (floor_temperature < heat_schedule[Day_Event_Next][0].temperature))))


				{
					relay_flag = 1;
					RELAY_ON;
					symbol_on(ICON_HEAT);
					Timer_min = 0;
					if(sensor_set & 0x02)
						preheat_begin_temperature = floor_temperature;
				}

				if((relay_flag) && ( ((!flag_preheat)&&(sensor_set&0x02)&&(floor_temperature > set_floor_temperature_prog))||
									   (flag_preheat &&(sensor_set&0x02)&&(Event_now+1<3)  &&  (floor_temperature  >  heat_schedule[Day_Event_Next][Event_next].temperature))||
									   (flag_preheat &&(sensor_set&0x02)&&(Event_now==3)   &&  (floor_temperature  >  heat_schedule[Day_Event_Next][0].temperature))||
													 (((sensor_set&0x03)==1)  &&  ((air_temperature+(delta_air_temperature-5))  >  set_floor_temperature_prog))||
													 (((sensor_set&0x03)==3)  &&  ((air_temperature+(delta_air_temperature-5))  >  set_air_temperature))))

				{

					relay_flag = 0;
					RELAY_OFF;
					symbol_off(ICON_HEAT);
					flag_preheat = 0;
					
				}

			}
			else
			{
				if ((!sensor_err) && (!relay_flag) && 			 (((!flag_preheat) && (
			                                                     (((sensor_set & 0x03) == 3) && ((air_temperature-(5-delta_air_temperature)) < set_air_temperature) && (floor_temperature < set_floor_temperature_prog)) ||
			                                                     (((sensor_set & 0x03) == 2) && (floor_temperature < set_floor_temperature_prog)) ||
			                                                     (((sensor_set & 0x03) == 1) && ((air_temperature-(5-delta_air_temperature)) < set_floor_temperature_prog)))) ||

																 (flag_preheat && ((sensor_set & 0x02) == 2) && (Event_now + 1 < 3) && (floor_temperature < heat_schedule[Day_Event_Next][Event_next].temperature)) ||
																 (flag_preheat && ((sensor_set & 0x02) == 2) && (Event_now == 3) && (floor_temperature < heat_schedule[Day_Event_Next][0].temperature))))


			    {
					relay_flag = 1;
					RELAY_ON;
					symbol_on(ICON_HEAT);
					Timer_min = 0;
					if (sensor_set & 0x02)
						preheat_begin_temperature = floor_temperature;

				}

			        if((relay_flag) &&( ((!flag_preheat)&&(sensor_set&0x02)&&(floor_temperature>set_floor_temperature_prog))||
			                          (flag_preheat &&(sensor_set&0x02)&&(Event_now+1<3)&&(floor_temperature>heat_schedule[Day_Event_Next][Event_next].temperature))||
			                          (flag_preheat &&(sensor_set&0x02)&&(Event_now==3)&&(floor_temperature>heat_schedule[Day_Event_Next][0].temperature))||
			                                        (((sensor_set&0x03)==1)&&((air_temperature-(5-delta_air_temperature))>set_floor_temperature_prog))||
			                                        (((sensor_set&0x03)==3)&&((air_temperature-(5-delta_air_temperature))>set_air_temperature))))

				{

					relay_flag = 0;
					RELAY_OFF;
					symbol_off(ICON_HEAT);
					flag_preheat = 0;
				}

			}




			if((sensor_set & 0x02) && (!sensor_err) && relay_flag && Timer_min && (floor_temperature > (preheat_begin_temperature)))
			{
				if(((preheat_timetable[preheat_begin_temperature - 5] > Timer_min) && (preheat_timetable[preheat_begin_temperature - 5] - Timer_min > 5)) || ((preheat_timetable[preheat_begin_temperature - 5] < Timer_min) && (Timer_min - preheat_timetable[preheat_begin_temperature - 5] > 5))) // ���� ���������� �� ���� ����� �� ������������
				{
					if(Timer_min < 60)
						preheat_timetable[preheat_begin_temperature - 5] = Timer_min;
					else
						preheat_timetable[preheat_begin_temperature - 5] = 60; //
				}
				preheat_begin_temperature = floor_temperature;
				Timer_min = 0;
			}

/* 			if(sensor_err && (WINDOW != STANDBY))
			{
				temp_time_relay = 0;
				relay_flag = 0;
				RELAY_OFF;
				WINDOW = ATT_MESG;
				conditions_register = (conditions_register | ALARM);
			} */

		}
	//===============================================================================================
		datetime_temperature();
		wifi_rssi();	
		//printf("DEBUG READ_TEMP TASK %d\n", Event_now);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}

//==================================================================================================================================
//=================================================== WORKFLOW =====================================================================
//==================================================================================================================================
void interface_task(void *arg) // Counter 10ms
{
	while (1)
	{
		if (counter_calib == 100)
		{
			if ((!touch_button_activated[BUTTON_MINUS]) 
			 && (!touch_button_activated[BUTTON_PLUS]) 
			 && (!touch_button_activated[BUTTON_MODE_OK]) 
			 && (!touch_button_activated[BUTTON_ON_OFF]) 
			 && (!touch_button_activated[BUTTON_SET_CANCEL]))
			{
				counter_calib = 0;
				touch_pad_read_filtered(BUTTON_MINUS, &touch_value);
				//printf("(%d, %d)  ", touch_button_no_use[BUTTON_MINUS], touch_value);
				//if (((touch_button_no_use[BUTTON_MINUS] + 10) > (touch_value)))
				{					
					touch_button_no_use[BUTTON_MINUS] = touch_value;
					ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_MINUS, touch_value * TOUCH_THRESH_PERCENT / 100));
				}

				touch_pad_read_filtered(BUTTON_MODE_OK, &touch_value);
				//printf("(%d, %d)   ", touch_button_no_use[BUTTON_MODE_OK], touch_value);
				//if (((touch_button_no_use[BUTTON_MODE_OK] + 10) > (touch_value)))
				{					
					touch_button_no_use[BUTTON_MODE_OK] = touch_value;
					ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_MODE_OK, touch_value * TOUCH_THRESH_PERCENT / 100));
				}				

				touch_pad_read_filtered(BUTTON_ON_OFF, &touch_value);
				//printf("(%d, %d)   ", touch_button_no_use[BUTTON_ON_OFF], touch_value);
				//if (((touch_button_no_use[BUTTON_ON_OFF] + 10) > (touch_value)))
				{					
					touch_button_no_use[BUTTON_ON_OFF] = touch_value;
					ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_ON_OFF, touch_value * TOUCH_THRESH_PERCENT / 100));
				}

				touch_pad_read_filtered(BUTTON_SET_CANCEL, &touch_value);
				//printf("(%d, %d)\n", touch_button_no_use[BUTTON_SET_CANCEL], touch_value);
				//if (((touch_button_no_use[BUTTON_SET_CANCEL] + 10) > (touch_value)))
				{					
					touch_button_no_use[BUTTON_SET_CANCEL] = touch_value;
					ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_SET_CANCEL, touch_value * TOUCH_THRESH_PERCENT / 100));
				}

				touch_pad_read_filtered(BUTTON_PLUS, &touch_value);
				//printf("(%d, %d)   ", touch_button_no_use[BUTTON_PLUS], touch_value);
				//if (((touch_button_no_use[BUTTON_PLUS] + 10) > (touch_value)))
				{
					touch_button_no_use[BUTTON_PLUS] = touch_value;
					ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_PLUS, touch_value * TOUCH_THRESH_PERCENT / 100));
				}

/* 				printf("======================== DEBUG THRESH HOLD ======================\n");
				printf("%d    %d   %d   %d   %d\n", touch_button_no_use[2],
					   								touch_button_no_use[4],
					   								touch_button_no_use[5],
					   								touch_button_no_use[7],
					   								touch_button_no_use[8]);
				

				uint16_t no_use_val[5] = {0,0,0,0,0};
				touch_pad_get_thresh(2, &no_use_val[0]);
				touch_pad_get_thresh(4, &no_use_val[1]);
				touch_pad_get_thresh(5, &no_use_val[2]);
				touch_pad_get_thresh(7, &no_use_val[3]);
				touch_pad_get_thresh(8, &no_use_val[4]);
				printf("%d    %d   %d   %d   %d\n", no_use_val[0],
					   								no_use_val[1],
					   								no_use_val[2],
					   								no_use_val[3],
					   								no_use_val[4]);
				printf("=================================================================\n"); */
			}
		}
		else
		{
			counter_calib++;
		}

		if (touch_button_activated[BUTTON_MINUS])
		{
			counter_calib = 0;

			if (BT_MINUS_PAR.counter == BT_MINUS_PAR.clock_dlong_click)
			{
				BT_MINUS_PAR.flag_click = 1;
				BT_MINUS_PAR.type_click = DLONG_CLICK;
			}
			else if (BT_MINUS_PAR.counter == BT_MINUS_PAR.clock_long_click)
			{
				BT_MINUS_PAR.flag_click = 1;
				BT_MINUS_PAR.type_click = LONG_CLICK;
			}
			else if (BT_MINUS_PAR.counter == BT_MINUS_PAR.clock_short_click)
			{
				BT_MINUS_PAR.flag_click = 1;
				BT_MINUS_PAR.type_click = SHORT_CLICK;
			}
			else if (BT_MINUS_PAR.counter == BT_MINUS_PAR.clock_dshort_click)
			{
				BT_MINUS_PAR.flag_click = 1;
				BT_MINUS_PAR.type_click = DSHORT_CLICK;
			}				

			if (BT_MINUS_PAR.counter < BT_MINUS_PAR.clock_dlong_click)
				BT_MINUS_PAR.counter++;
		}
		else
		{
			BT_MINUS_PAR.flag_click = 0;
			BT_MINUS_PAR.counter = 0;
		}


		if (touch_button_activated[BUTTON_PLUS])
		{
			counter_calib = 0;

			if (BT_PLUS_PAR.counter == BT_PLUS_PAR.clock_dlong_click)
			{
				BT_PLUS_PAR.flag_click = 1;
				BT_PLUS_PAR.type_click = DLONG_CLICK;
			}
			else if (BT_PLUS_PAR.counter == BT_PLUS_PAR.clock_long_click)
			{
				BT_PLUS_PAR.flag_click = 1;
				BT_PLUS_PAR.type_click = LONG_CLICK;
			}
			else if (BT_PLUS_PAR.counter == BT_PLUS_PAR.clock_short_click)
			{
				BT_PLUS_PAR.flag_click = 1;
				BT_PLUS_PAR.type_click = SHORT_CLICK;
			}
			else if (BT_PLUS_PAR.counter == BT_PLUS_PAR.clock_dshort_click)
			{
				BT_PLUS_PAR.flag_click = 1;
				BT_PLUS_PAR.type_click = DSHORT_CLICK;
			}

			if (BT_PLUS_PAR.counter < BT_PLUS_PAR.clock_dlong_click)
				BT_PLUS_PAR.counter++;
		}
		else
		{
			BT_PLUS_PAR.flag_click = 0;
			BT_PLUS_PAR.counter = 0;
		}

		if (touch_button_activated[BUTTON_MODE_OK])
		{
			counter_calib = 0;

			if (BT_MODE_OK_PAR.counter == BT_MODE_OK_PAR.clock_dlong_click)
			{
				BT_MODE_OK_PAR.flag_click = 1;
				BT_MODE_OK_PAR.type_click = DLONG_CLICK;
			}
			else if (BT_MODE_OK_PAR.counter == BT_MODE_OK_PAR.clock_long_click)
			{
				BT_MODE_OK_PAR.flag_click = 1;
				BT_MODE_OK_PAR.type_click = LONG_CLICK;
			}
			else if (BT_MODE_OK_PAR.counter == BT_MODE_OK_PAR.clock_short_click)
			{
				BT_MODE_OK_PAR.flag_click = 1;
				BT_MODE_OK_PAR.type_click = SHORT_CLICK;
			}
			else if (BT_MODE_OK_PAR.counter == BT_MODE_OK_PAR.clock_dshort_click)
			{
				BT_MODE_OK_PAR.flag_click = 1;
				BT_MODE_OK_PAR.type_click = DSHORT_CLICK;
			}

			if (BT_MODE_OK_PAR.counter < BT_MODE_OK_PAR.clock_dlong_click)
				BT_MODE_OK_PAR.counter++;
		}
		else
		{
			BT_MODE_OK_PAR.flag_click = 0;
			BT_MODE_OK_PAR.counter = 0;

		}

		if (touch_button_activated[BUTTON_ON_OFF])
		{
			counter_calib = 0;

			if (BT_ON_OFF_PAR.counter == BT_ON_OFF_PAR.clock_dlong_click)
			{
				BT_ON_OFF_PAR.flag_click = 1;
				BT_ON_OFF_PAR.type_click = DLONG_CLICK;
			}
			else if (BT_ON_OFF_PAR.counter == BT_ON_OFF_PAR.clock_long_click)
			{
				BT_ON_OFF_PAR.flag_click = 1;
				BT_ON_OFF_PAR.type_click = LONG_CLICK;
			}
			else if (BT_ON_OFF_PAR.counter == BT_ON_OFF_PAR.clock_short_click)
			{
				BT_ON_OFF_PAR.flag_click = 1;
				BT_ON_OFF_PAR.type_click = SHORT_CLICK;
			}
			else if (BT_ON_OFF_PAR.counter == BT_ON_OFF_PAR.clock_dshort_click)
			{
				BT_ON_OFF_PAR.flag_click = 1;
				BT_ON_OFF_PAR.type_click = DSHORT_CLICK;
			}

			if (BT_ON_OFF_PAR.counter < BT_ON_OFF_PAR.clock_dlong_click)
				BT_ON_OFF_PAR.counter++;
		}
		else
		{
			BT_ON_OFF_PAR.flag_click = 0;
			BT_ON_OFF_PAR.counter = 0;
		}

		if (touch_button_activated[BUTTON_SET_CANCEL])
		{
			counter_calib = 0;

			if (BT_SET_CANCEL_PAR.counter == BT_SET_CANCEL_PAR.clock_dlong_click)
			{
				BT_SET_CANCEL_PAR.flag_click = 1;
				BT_SET_CANCEL_PAR.type_click = DLONG_CLICK;
			}
			else if (BT_SET_CANCEL_PAR.counter == BT_SET_CANCEL_PAR.clock_long_click)
			{
				BT_SET_CANCEL_PAR.flag_click = 1;
				BT_SET_CANCEL_PAR.type_click = LONG_CLICK;
			}
			else if (BT_SET_CANCEL_PAR.counter == BT_SET_CANCEL_PAR.clock_short_click)
			{
				BT_SET_CANCEL_PAR.flag_click = 1;
				BT_SET_CANCEL_PAR.type_click = SHORT_CLICK;
			}
			else if (BT_SET_CANCEL_PAR.counter == BT_SET_CANCEL_PAR.clock_dshort_click)
			{
				BT_SET_CANCEL_PAR.flag_click = 1;
				BT_SET_CANCEL_PAR.type_click = DSHORT_CLICK;
			}

			if (BT_SET_CANCEL_PAR.counter < BT_SET_CANCEL_PAR.clock_dlong_click)
				BT_SET_CANCEL_PAR.counter++;
		}
		else
		{
			BT_SET_CANCEL_PAR.flag_click = 0;
			BT_SET_CANCEL_PAR.counter = 0;
		}
		

/* 		printf("%d %d %d %d %d\n", 	BT_MINUS_PAR.type_click, 
									BT_MODE_OK_PAR.type_click, 
									BT_ON_OFF_PAR.type_click, 
									BT_SET_CANCEL_PAR.type_click, 
									BT_PLUS_PAR.type_click); */

/*    	printf("%d %d %d %d %d\n", touch_button_activated[BUTTON_MINUS], 
								   touch_button_activated[BUTTON_PLUS], 
								   touch_button_activated[BUTTON_MODE_OK], 
								   touch_button_activated[BUTTON_ON_OFF], 
								   touch_button_activated[BUTTON_SET_CANCEL]);  */  

 		touch_button_activated[BUTTON_MINUS] 		= 0;
		touch_button_activated[BUTTON_PLUS] 		= 0;
		touch_button_activated[BUTTON_MODE_OK] 		= 0;
		touch_button_activated[BUTTON_ON_OFF] 		= 0;
		touch_button_activated[BUTTON_SET_CANCEL] 	= 0;
		
		device_menu();
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
//==================================================================================================================================

//==================================================================================================================================
//=================================================== LCD TASK =====================================================================
//==================================================================================================================================
void lcd_task(void *arg)
{
	uint8_t send_to_lcd_buff[LCD_BUFF_SIZE];
	while(1)
	{
		xQueueReceive(xlcd_data_queue, &send_to_lcd_buff, portMAX_DELAY);
		send_data_to_lcd(send_to_lcd_buff);
	}
}
//==================================================================================================================================

void timer_500(void* arg)
{
	//---------------------------------------------------------------------------------------------------------------------------------- BLINKING
	if ((!BT_PLUS_PAR.flag_click) && (!BT_MINUS_PAR.flag_click) && (!BT_MODE_OK_PAR.flag_click) && (!BT_ON_OFF_PAR.flag_click) && (!BT_SET_CANCEL_PAR.flag_click))
	{
		blinking = !blinking;
	}
	else
	{
		blinking = pdTRUE;
	}
	//---------------------------------------------------------------------------------------------------------------------------------- BACKLIGHT/LOCK 40S
	if (bkl_led_off_timer)
	{
		bkl_led_off_timer--;

		if (bkl_led_off_timer == 0)
		{
			if(device_status == DEVICE_MANUAL_CONFIG) 		
			{
				conf_step = 15;
				clear_LCD();
			}
			else
			{
				device_status = DEVICE_SCREEN_LOCK;
				lcd_redraw();
			}
			LED_OFF;
			vTaskResume(xTask_read_temp_handle);
		}
	}

	//---------------------------------------------------------------------------------------------------------------------------------- TIME
	if (device_status == DEVICE_SCREEN_LOCK)
	{
		RTC_task();
	}
	//---------------------------------------------------------------------------------------------------------------------------------- 
}

void timer_1000(void* arg)
{	
		
	if (relay_flag)
	{
		if (RTC_TimeStr.RTC_Seconds == 0)
		{
			relay_operating_time_tmp++;
			printf("relay_operating_time_tmp %d\n", relay_operating_time_tmp);
		}

		if ((RTC_TimeStr.RTC_Seconds == 0) && (RTC_TimeStr.RTC_Minutes == 0)) //save time_relay every our
		{
			relay_operating_time += relay_operating_time_tmp;
			//printf("relay_operating_time %d\n",relay_operating_time);
			nvs_open("storage", NVS_READWRITE, &my_handle);
			nvs_set_u32(my_handle, "Time_relay", relay_operating_time);
			nvs_commit(my_handle);
			nvs_close(my_handle);

			relay_operating_time_tmp = 0;
		}
	}
	else if ((!relay_flag) && (relay_operating_time_tmp))
	{
		relay_operating_time += relay_operating_time_tmp;
		//printf("relay_operating_time %d\n",relay_operating_time);
		nvs_open("storage", NVS_READWRITE, &my_handle);
		nvs_set_u32(my_handle, "Time_relay", relay_operating_time);
		nvs_commit(my_handle);
		nvs_close(my_handle);

		relay_operating_time_tmp = 0;

		if (relay_time_ours == 65535)
		{
			relay_operating_time = 0;
			nvs_open("storage", NVS_READWRITE, &my_handle);
			nvs_set_u32(my_handle, "Time_relay", relay_operating_time);
			nvs_commit(my_handle);
			nvs_close(my_handle);
			relay_time_ours = 0;
			relay_time_minutes = 0;
			relay_operating_time_tmp = 0;
		}
	}

	if(small_output_timer)
	{
		small_output_timer--;
	}
	else
	{
		datetime_temperature_fl = !datetime_temperature_fl;
		small_output_timer = 5;
	}
	
}


void connect_task(void *arg)
{
	uint8_t *conn_mode;
	conn_mode = (uint8_t*)arg;

	uint8_t counter_back = 60;
	while(1)
	{
		if(conn_mode)
		{
			large_digits(1112);
		}
		else
		{
			large_digits(511);
		}

		small_digit(6, counter_back / 10);
		small_digit(7, counter_back % 10);

		counter_back--;
		if(!counter_back) 
		{
			LED_OFF;
			esp_restart();
		}

		if(WIFI)
		{
        	clear_LCD();
        	lcd_redraw();
			vTaskDelete(connect_task_handle);
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
//==================================================================================================================================
//==================================================== MAIN ========================================================================
//==================================================================================================================================
void app_main()
{
	//---------------------------------- Varables
	//datetime_temperature_fl = 1;
	calib_vref = 1050;
	full_year = 2018;
	small_output_timer = 5;
	tmp_Event_now = 4;
	//----------------------------------


    //----------------------Buttons init--------------------------------------------
 	BT_PLUS_PAR.flag_click			 		= 0;
	BT_PLUS_PAR.counter				 		= 0;
	BT_PLUS_PAR.type_click			 		= NO_CLICK;
	BT_PLUS_PAR.clock_short_click  	 		= COUNT_SHORT_CLICK;
	BT_PLUS_PAR.clock_dshort_click     		= COUNT_DSHORT_CLICK;
	BT_PLUS_PAR.clock_long_click   	 		= COUNT_LONG_CLICK;
	BT_PLUS_PAR.clock_dlong_click  	 		= COUNT_DLONG_CLICK;

	BT_MINUS_PAR.flag_click			 		= 0;
	BT_MINUS_PAR.counter					= 0;
	BT_MINUS_PAR.type_click			 		= NO_CLICK;
	BT_MINUS_PAR.clock_short_click	 		= COUNT_SHORT_CLICK;
	BT_MINUS_PAR.clock_dshort_click   		= COUNT_DSHORT_CLICK;
	BT_MINUS_PAR.clock_long_click 	 		= COUNT_LONG_CLICK;
	BT_MINUS_PAR.clock_dlong_click	 		= COUNT_DLONG_CLICK;

	BT_MODE_OK_PAR.flag_click				= 0;
	BT_MODE_OK_PAR.counter					= 0;
	BT_MODE_OK_PAR.type_click				= NO_CLICK;
	BT_MODE_OK_PAR.clock_short_click		= COUNT_SHORT_CLICK;
	BT_MODE_OK_PAR.clock_dshort_click 		= COUNT_DSHORT_CLICK;
	BT_MODE_OK_PAR.clock_long_click	 		= COUNT_LONG_CLICK;
	BT_MODE_OK_PAR.clock_dlong_click		= COUNT_DLONG_CLICK;

	BT_ON_OFF_PAR.flag_click				= 0;
	BT_ON_OFF_PAR.counter			 		= 0;
	BT_ON_OFF_PAR.type_click				= NO_CLICK;
	BT_ON_OFF_PAR.clock_short_click	 		= COUNT_SHORT_CLICK;
	BT_ON_OFF_PAR.clock_dshort_click  		= COUNT_DSHORT_CLICK;
	BT_ON_OFF_PAR.clock_long_click	 		= COUNT_LONG_CLICK;
	BT_ON_OFF_PAR.clock_dlong_click	 		= COUNT_DLONG_CLICK; 

	BT_SET_CANCEL_PAR.flag_click			= 0;
	BT_SET_CANCEL_PAR.counter				= 0;
	BT_SET_CANCEL_PAR.type_click			= NO_CLICK;
	BT_SET_CANCEL_PAR.clock_short_click	 	= COUNT_SHORT_CLICK;
	BT_SET_CANCEL_PAR.clock_dshort_click  	= COUNT_DSHORT_CLICK;
	BT_SET_CANCEL_PAR.clock_long_click	 	= COUNT_LONG_CLICK;
	BT_SET_CANCEL_PAR.clock_dlong_click	 	= COUNT_DLONG_CLICK; 
	//-----------------------------------------------------------------------------

	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}

	first_start = 1;
	nvs_open("storage", NVS_READWRITE, &my_handle);
	nvs_get_u8(my_handle, "first_start", &first_start);
	nvs_commit(my_handle);
	nvs_close(my_handle);
	
	if(first_start) // Check 1 start of device/after hardreset and restore factory settings
	{
		set_factory_settings();
		first_start = 0;
		nvs_open("storage", NVS_READWRITE, &my_handle);
		nvs_set_u8(my_handle, "first_start", first_start);
		nvs_commit(my_handle);
		nvs_close(my_handle);
	}
	load_from_flash(); // Load settings from flash

	set_prog_comfort_temp = heat_schedule[0][0].temperature; // Varables for device_status = DEVICE_MANUAL_CONFIG
	set_prog_eco_temp     = heat_schedule[0][1].temperature;

	device_gpio_init();
	beep_init(128);
	adc_init();
    init_i2c_rtc();
	touchinit();
	initialise_wifi();

	printf("\n");
    ESP_LOGW(TAG, "Startup..");
    ESP_LOGW(TAG, "Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGW(TAG, "IDF version: %s", esp_get_idf_version());
	ESP_LOGW(TAG, "Firmware version: %s", VERSION);
	printf("\n");

	//=============================================================== LCD var-s & task
	//xSPI_semaphore = xSemaphoreCreateMutex();
	xLCD_data_using_semaphore = xSemaphoreCreateMutex();
	xlcd_data_queue = xQueueCreate(5, LCD_BUFF_SIZE);
	xTaskCreate(&lcd_task,    "lcd_task",    4096, NULL, 10, NULL); //
	//================================================================================

//=====================================================================================
	//==================================================================== Timer 1 init
	const esp_timer_create_args_t periodic_timer_args1 =
		{
			.callback = &timer_500,
			.name = "periodic_500"
		};

	esp_timer_handle_t periodic_timer1;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args1, &periodic_timer1));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer1, 500000)); //period 500ms
	//=================================================================================

	//==================================================================== Timer 2 init
 	const esp_timer_create_args_t periodic_timer_args2 =
		{
			.callback = &timer_1000,
			.name = "periodic_1000"
		};

	esp_timer_handle_t periodic_timer2;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args2, &periodic_timer2));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer2, 1000000)); //period 1000ms
	//=================================================================================
//=====================================================================================

	RTC_task();
	EVENT();
	xTaskCreate(&interface_task,   "interface_task",   8196, NULL, 10, &xTask_interface_handle);
	xTaskCreate(&read_temperature, "read_temperature", 8196, NULL, 10, &xTask_read_temp_handle);

	vTaskDelay( 100 / portTICK_PERIOD_MS);
 	if(device_power_status)
	{
		device_status = DEVICE_SCREEN_LOCK;
		lcd_redraw();
	}
	else
	{
		device_status = DEVICE_OFF;
		vTaskSuspend(xTask_read_temp_handle);
		clear_LCD();
		RELAY_OFF;
		relay_flag = false;
	} 

}