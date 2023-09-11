#include "device_init.h"


//============================================================================================
//================================================================= Save settings from flash =
//============================================================================================
void set_factory_settings(void)
{
	nvs_open("storage", NVS_READWRITE, &my_handle);
	device_power_status = 0;
	nvs_set_u8(my_handle, "ON_OFF", device_power_status);
	//---------------------------------------------------------------------------------------- Heat table
	for (uint8_t k = 0; k < 7; k++) // 7 days counter
	{
		heat_schedule[k][0].event_time = 420;  		// 7.00
		heat_schedule[k][0].temperature = 28;	// 28C
		heat_schedule[k][1].event_time = 480;  		// 8.00
		heat_schedule[k][1].temperature = 12;	// 12C
		heat_schedule[k][2].event_time = 1140; 		// 19.00
		heat_schedule[k][2].temperature = 28;	// 28C
		heat_schedule[k][3].event_time = 1320; 		// 22.00
		heat_schedule[k][3].temperature = 12;	// 12C

		if(k < 5) work_weekend[k] = 0; // work days mon-fri
		else	  work_weekend[k] = 1; // days off 	sat-sun
	}

	size_t heat_schedule_len = 112; //sizeof(program)*4*7
	size_t work_weekend_len  = 7;
	nvs_set_blob(my_handle, "heat_schedule", (const void *)heat_schedule, heat_schedule_len);
	nvs_set_blob(my_handle,  "work_weekend", (const void *)work_weekend, work_weekend_len);
	//---------------------------------------------------------------------------------------- Preheat table
	for (uint8_t k = 0; k < 40; k++)
	{
		preheat_timetable[k] = 10;
	}
	size_t preheat_timetable_len = 0;
	preheat_timetable_len = sizeof(preheat_timetable);
	nvs_set_blob(my_handle, "preheat_tt", (const void *)preheat_timetable, preheat_timetable_len);
	//---------------------------------------------------------------------------------------- Heat mode
	heat_mode = 0;
	nvs_set_u8(my_handle, "heat_mode", heat_mode);
	//---------------------------------------------------------------------------------------- Set manual floor temp
	set_manual_floor_temperature = 20;
	nvs_set_u8(my_handle, "set_floor_temp", set_manual_floor_temperature);
	//---------------------------------------------------------------------------------------- Set antifreeze floor temp
	set_antifreeze_floor_temperature = 12;
	nvs_set_u8(my_handle, "set_af_temp", set_antifreeze_floor_temperature);
	//---------------------------------------------------------------------------------------- Set air temp
	set_air_temperature = 20;
	nvs_set_u8(my_handle, "set_air_temp", set_air_temperature);
	//---------------------------------------------------------------------------------------- Set delta air temp
	delta_air_temperature = 5;
	nvs_set_u8(my_handle, "delta_air_temp", delta_air_temperature);
	//---------------------------------------------------------------------------------------- Set sensor set 0x07 - all sensors + selftraining
	sensor_set = 0x02;
	nvs_set_u8(my_handle, "sensor_set", sensor_set);
	//---------------------------------------------------------------------------------------- Sensor type 0x01 - 6.8K
	sensor_type = 0x01;
	nvs_set_u8(my_handle, "sensor_type", sensor_type);
	//---------------------------------------------------------------------------------------- MQTT server IP (default 185.76.147.189)
	strcpy((char *)&MQTT_IP[0], "");
	nvs_set_str(my_handle, "MQTT_IP", MQTT_IP);
	//---------------------------------------------------------------------------------------- MQTT port (default 1883)
	strcpy((char *)&MQTT_Port[0], "");
	nvs_set_str(my_handle, "MQTT_Port", MQTT_Port);
	//---------------------------------------------------------------------------------------- MQTT UID
	strcpy((char *)&UID[0], "");
	nvs_set_str(my_handle, "UID", (const char *)UID);
	//---------------------------------------------------------------------------------------- WIFI SSID
	strcpy((char *)&SSID[0], "");
	nvs_set_str(my_handle, "SSID", (const char *)SSID);
	//---------------------------------------------------------------------------------------- WIFI PASS
	strcpy((char *)&PASS[0], "");
	nvs_set_str(my_handle, "PASS", (const char *)PASS);
	//---------------------------------------------------------------------------------------- First link flag (def 0)
	first_link = 0;
	nvs_set_u8(my_handle, "first_link", first_link);
	//----------------------------------------------------------------------------------------
	first_start = 1;
	nvs_open("storage", NVS_READWRITE, &my_handle);
	nvs_set_u8(my_handle, "first_start", first_start);
	//----------------------------------------------------------------------------------------

	nvs_commit(my_handle);
	nvs_close(my_handle);
}

//============================================================================================
//================================================================= Load settings from flash =
//============================================================================================
void load_from_flash(void)
{
	nvs_open("storage", NVS_READWRITE, &my_handle);
	
	nvs_get_u8(my_handle, "ON_OFF", &device_power_status);
	//---------------------------------------------------------------------------------------- Heat table
	size_t required_size_p = 0;
	nvs_get_blob(my_handle, "heat_schedule", NULL, &required_size_p);
	nvs_get_blob(my_handle, "heat_schedule", &heat_schedule, &required_size_p);

	required_size_p = 0;
	nvs_get_blob(my_handle, "work_weekend", NULL, &required_size_p);
	nvs_get_blob(my_handle, "work_weekend", &work_weekend, &required_size_p);
	//---------------------------------------------------------------------------------------- Preheat table
	required_size_p = 0;
	nvs_get_blob(my_handle, "preheat_tt", NULL, &required_size_p);
	nvs_get_blob(my_handle, "preheat_tt", &preheat_timetable, &required_size_p);
	//---------------------------------------------------------------------------------------- Heat mode
	nvs_get_u8(my_handle, "heat_mode", &heat_mode);
	//---------------------------------------------------------------------------------------- Set floor temp
	nvs_get_u8(my_handle, "set_floor_temp", &set_manual_floor_temperature);
	//---------------------------------------------------------------------------------------- Set af temp
	nvs_get_u8(my_handle, "set_af_temp", &set_antifreeze_floor_temperature);
	//---------------------------------------------------------------------------------------- Set air temp
	nvs_get_u8(my_handle, "set_air_temp", &set_air_temperature);
	//---------------------------------------------------------------------------------------- Set delta air temp
	nvs_get_u8(my_handle, "delta_air_temp", &delta_air_temperature);
	//---------------------------------------------------------------------------------------- Set sensor set 0x07 - all sensors + selftraining
	nvs_get_u8(my_handle, "sensor_set", &sensor_set);
	//---------------------------------------------------------------------------------------- Sensor type 0x01 - 6.8K
	nvs_get_u8(my_handle, "sensor_type", &sensor_type);
	//---------------------------------------------------------------------------------------- MQTT server IP (default 185.76.147.189)
	size_t mqtt_ip_len = 0;
	nvs_get_str(my_handle, "MQTT_IP", NULL, &mqtt_ip_len);
	nvs_get_str(my_handle, "MQTT_IP", (char *)&MQTT_IP[0], &mqtt_ip_len);
	//---------------------------------------------------------------------------------------- MQTT port (default 1883)
	size_t mqtt_port_len = 0;
	nvs_get_str(my_handle, "MQTT_Port", NULL, &mqtt_port_len);
	nvs_get_str(my_handle, "MQTT_Port", (char *)&MQTT_Port[0], &mqtt_port_len);
	//---------------------------------------------------------------------------------------- MQTT UID
	size_t mqtt_uid_len = 0;
	nvs_get_str(my_handle, "UID", NULL, &mqtt_uid_len);
	nvs_get_str(my_handle, "UID", (char *)&UID[0], &mqtt_uid_len);
	//printf("uid %s\n",UID);
	//---------------------------------------------------------------------------------------- WIFI SSID
	size_t mqtt_ssid_len = 0;
	nvs_get_str(my_handle, "SSID", NULL, &mqtt_ssid_len);
	nvs_get_str(my_handle, "SSID", (char *)&SSID[0], &mqtt_ssid_len);
	//---------------------------------------------------------------------------------------- WIFI PASS
	size_t mqtt_pass_len = 0;
	nvs_get_str(my_handle, "PASS", NULL, &mqtt_pass_len);
	nvs_get_str(my_handle, "PASS", (char *)&PASS[0], &mqtt_pass_len);
	//----------------------------------------------------------------------------------------
	nvs_commit(my_handle);
	nvs_close(my_handle);
}

//============================================================================================
//====================================================================== Beep initialization =
//============================================================================================
void beep_init(uint32_t lcd_duty)
{
    //----------------------------Beeper init pwm mode-------------------------------
	ledc_timer_config_t timer_conf;
	timer_conf.duty_resolution = LEDC_TIMER_12_BIT;
	timer_conf.freq_hz = 4000;
	timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
	timer_conf.timer_num = LEDC_TIMER_0;
	ledc_timer_config(&timer_conf);

	ledc_channel_config_t ledc_conf;
	ledc_conf.channel = LEDC_CHANNEL_5;
	ledc_conf.duty = lcd_duty;
	ledc_conf.gpio_num = BEEP_GPIO;
	ledc_conf.intr_type = LEDC_INTR_DISABLE;
	ledc_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_conf.timer_sel = LEDC_TIMER_0;
	ledc_conf.hpoint = 0;
	ledc_channel_config(&ledc_conf);
	ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
	//------------------------------------------------------------------------------------------
}

void beep_beep()
{
	ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
	vTaskDelay( BEEP_DELAY / portTICK_PERIOD_MS);
	ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
}

//============================================================================================
//====================================================================== GPIO initialization =
//============================================================================================
void device_gpio_init()
{
	gpio_config_t gpioConfig;
	gpioConfig.pin_bit_mask = (1 << 4) | (1 << 9) | (1 << 19) | (1 << 22);
	gpioConfig.mode = GPIO_MODE_OUTPUT;
	gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
	gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
	gpioConfig.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&gpioConfig);
}

//============================================================================================
//======================================================================= ADC initialization =
//============================================================================================
void adc_init()
{
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_11); // CALIB
	adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11); // FLOOR
	adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11); // AIR

	adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, calib_vref, adc_chars);

	if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
    //esp_adc_cal_get_characteristics(3300, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, &characteristics);
    //SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_CYCLE,255, SENS_SAR1_SAMPLE_CYCLE_S);
    //SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_NUM, 1, SENS_SAR1_SAMPLE_NUM_S);
    //SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_CLK_DIV, 1, SENS_SAR1_CLK_DIV_S);
}

//==========================================================================================
//================================================================================= I2C init
//==========================================================================================
void init_i2c_rtc()
{
 	i2c_config_t rtc_i2c;
	rtc_i2c.mode = I2C_MODE_MASTER;
	rtc_i2c.sda_io_num = RTC_SDA_PIN;
	rtc_i2c.scl_io_num = RTC_SCL_PIN;
	rtc_i2c.sda_pullup_en = GPIO_PULLUP_ENABLE;
	rtc_i2c.scl_pullup_en = GPIO_PULLUP_ENABLE;
	rtc_i2c.master.clk_speed = 400000;
	i2c_param_config(RTC_I2C, &rtc_i2c);
	i2c_driver_install(RTC_I2C, I2C_MODE_MASTER, 0, 0, 0); 

	i2c_config_t lcd_i2c;
	lcd_i2c.mode = I2C_MODE_MASTER;
	lcd_i2c.sda_io_num = LCD_SDA_PIN;
	lcd_i2c.scl_io_num = LCD_SCL_PIN;
	lcd_i2c.sda_pullup_en = GPIO_PULLUP_ENABLE;
	lcd_i2c.scl_pullup_en = GPIO_PULLUP_ENABLE;
	lcd_i2c.master.clk_speed = 400000;
	i2c_param_config(LCD_I2C, &lcd_i2c);
	i2c_driver_install(LCD_I2C, I2C_MODE_MASTER, 0, 0, 0);
}


//==========================================================================================
//=============================================================================== Touch init
//==========================================================================================
void touch_button_interrupt(void * arg)
{
    uint32_t touch_pad_mask = touch_pad_get_status();
    //clear interrupt
    touch_pad_clear_status();
	
    if ((touch_pad_mask >> BUTTON_MINUS) & 0x01) 
	{
        touch_button_activated[BUTTON_MINUS] = 1;
    }

	if ((touch_pad_mask >> BUTTON_PLUS) & 0x01) 
	{
        touch_button_activated[BUTTON_PLUS] = 1;
    }

	if ((touch_pad_mask >> BUTTON_MODE_OK) & 0x01) 
	{
        touch_button_activated[BUTTON_MODE_OK] = 1;
    }

	if ((touch_pad_mask >> BUTTON_ON_OFF) & 0x01) 
	{
        touch_button_activated[BUTTON_ON_OFF] = 1;
    }

	if ((touch_pad_mask >> BUTTON_SET_CANCEL) & 0x01) 
	{
        touch_button_activated[BUTTON_SET_CANCEL] = 1;
    }		
    
}

void touchinit(void)
{
	printf("Initializing touch pad\n");
	touch_pad_init();
	touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
	// Initialize and start a software filter to detect slight change of capacitance.
	//touch_pad_set_filter_period(10);
	// Set measuring time and sleep time
	// In this case, measurement will sustain 0xffff / 8Mhz = 8.19ms
	// Meanwhile, sleep time between two measurement will be 0x1000 / 150Khz = 27.3 ms
	touch_pad_set_meas_time(0x0100, 0xffff);

	touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_0V5); /// 0.2V ON/OFF

	touch_pad_config(BUTTON_MINUS, 		TOUCH_THRESH_NO_USE);
	touch_pad_config(BUTTON_PLUS, 		TOUCH_THRESH_NO_USE);
	touch_pad_config(BUTTON_MODE_OK, 	TOUCH_THRESH_NO_USE);
	touch_pad_config(BUTTON_ON_OFF, 	TOUCH_THRESH_NO_USE);
	touch_pad_config(BUTTON_SET_CANCEL, TOUCH_THRESH_NO_USE);

	touch_pad_filter_start(200);

	vTaskDelay(1000 / portTICK_PERIOD_MS);
		
	touch_pad_read_filtered(BUTTON_MINUS, &touch_value);
	touch_button_no_use[BUTTON_MINUS] = touch_value;
	printf("BUTTON_MINUS:		%d\n", touch_button_no_use[BUTTON_MINUS]);
	//set interrupt threshold.
	ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_MINUS, touch_button_no_use[BUTTON_MINUS] * TOUCH_THRESH_PERCENT / 100));

	touch_pad_read_filtered(BUTTON_MODE_OK, &touch_value);
	touch_button_no_use[BUTTON_MODE_OK] = touch_value;
	printf("BUTTON_MODE_OK:		%d\n", touch_button_no_use[BUTTON_MODE_OK]);
	//set interrupt threshold.
	ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_MODE_OK, touch_button_no_use[BUTTON_MODE_OK] * TOUCH_THRESH_PERCENT / 100));	

	touch_pad_read_filtered(BUTTON_ON_OFF, &touch_value);
	touch_button_no_use[BUTTON_ON_OFF] = touch_value;
	printf("BUTTON_ON_OFF:		%d\n", touch_button_no_use[BUTTON_ON_OFF]);
	//set interrupt threshold.
	ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_ON_OFF, touch_button_no_use[BUTTON_ON_OFF] * TOUCH_THRESH_PERCENT / 100));	

	touch_pad_read_filtered(BUTTON_SET_CANCEL, &touch_value);
	touch_button_no_use[BUTTON_SET_CANCEL] = touch_value;
	printf("BUTTON_SET_CANCEL: 	%d\n", touch_button_no_use[BUTTON_SET_CANCEL]);
	//set interrupt threshold.
	ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_SET_CANCEL, touch_button_no_use[BUTTON_SET_CANCEL] * TOUCH_THRESH_PERCENT / 100));	

	touch_pad_read_filtered(BUTTON_PLUS, &touch_value);
	touch_button_no_use[BUTTON_PLUS] = touch_value;
	printf("BUTTON_PLUS:		%d\n", touch_button_no_use[BUTTON_PLUS]);
	//set interrupt threshold.
	ESP_ERROR_CHECK(touch_pad_set_thresh(BUTTON_PLUS, touch_button_no_use[BUTTON_PLUS] * TOUCH_THRESH_PERCENT / 100));

	touch_pad_isr_register(touch_button_interrupt, NULL);
	touch_pad_intr_enable();
}