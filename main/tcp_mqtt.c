
#include "device_init.h"
#include "tcp_mqtt.h"


void buffer_add_char(uint8_t new_char, uint16_t position)
{
	Buffer[position] = new_char;
}

void buffer_add_string(uint8_t *new_part, uint16_t position, uint16_t count)
{
	uint8_t sym;
	while (count--)
	{
		sym = *new_part++;
		Buffer[position++] = sym;
	}
}

void buffer_add_int(uint16_t new_int, uint16_t position)
{
	Buffer[position + 1] = new_int;
	Buffer[position] = (new_int >> 8);
}

uint16_t EX_Send_Config(uint16_t pos)
{
    uint8_t j, i;
    uint8_t access_status = 0;
    volatile uint16_t pos_temp = pos;
    if (Access_flag == 1) 
        access_status = 0;
    buffer_add_string((uint8_t *)"\x02TAR\x00\x00", pos_temp, 6); // instruction set + 6 bytes length
    pos_temp += 6;
    //--------------------------------------
    buffer_add_string((uint8_t *)"T\x00\x04", pos_temp, 3); // Current temperature
    pos_temp += 3;
    if (sensor_err)
        buffer_add_char(0x81, pos_temp++);
    else
        buffer_add_char(floor_temperature, pos_temp++);
    if (delta_air_temperature > 5)
    {
        buffer_add_char(air_temperature + (delta_air_temperature - 5), pos_temp++);
    }
    else
    {
        buffer_add_char(air_temperature - (5 - delta_air_temperature), pos_temp++);
    }
    buffer_add_char(Day_Event_Now, pos_temp++); // need to add day of event
    buffer_add_char(Event_now, pos_temp++);     // need to add event now
    //--------------------------------------
    //buffer_add_string((uint8_t *)"S\x00\x08", pos_temp, 3); // Settings
    buffer_add_string((uint8_t *)"S\x00\x06", pos_temp, 3); // Settings
    pos_temp += 3;
    buffer_add_char(device_power_status, pos_temp++);
    buffer_add_char(heat_mode, pos_temp++); //heating mode
    buffer_add_char(sensor_set, pos_temp++);
    //week_byte = array_to_byte();
    //buffer_add_char(week_byte, pos_temp++);
    buffer_add_char(set_manual_floor_temperature, pos_temp++);
    buffer_add_char(set_air_temperature, pos_temp++);
    //buffer_add_char(set_antifreeze_floor_temperature, pos_temp++);
    buffer_add_char((int8_t)(delta_air_temperature - 5), pos_temp++);
    //--------------------------------------
    buffer_add_string((uint8_t *)"C\x00\x54", pos_temp, 3); // Heating table
    pos_temp += 3;
    for (i = 0; i < 7; i++)
        for (j = 0; j < 4; j++)
        {
            buffer_add_int(heat_schedule[i][j].event_time, pos_temp);
            pos_temp += 2;
            buffer_add_char(heat_schedule[i][j].temperature, pos_temp++);
        }
    //--------------------------------------
    buffer_add_string((uint8_t *)"I\x00\x05", pos_temp, 3); 
    pos_temp += 3;
    //buffer_add_string((uint8_t *)"T4", pos_temp, 2); // Identification
    buffer_add_string((uint8_t *)"E1", pos_temp, 2); // Identification
    pos_temp += 2;
    buffer_add_string((uint8_t *)VERSION, pos_temp, 3);
    pos_temp += 3;
    //--------------------------------------
    buffer_add_string((uint8_t *)"M\x00\x11", pos_temp, 3); // MAC
    pos_temp += 3;
    buffer_add_string((uint8_t *)MAC_esp, pos_temp, 17);
    pos_temp += 17;
    //--------------------------------------
    buffer_add_string((uint8_t *)"A\x00\x01", pos_temp, 3); // Access status
    pos_temp += 3;
    buffer_add_char(access_status, pos_temp++);
    //--------------------------------------
    buffer_add_string((uint8_t *)"Z\x00\x01", pos_temp, 3); // Type of sensor
    pos_temp += 3;
    buffer_add_char(sensor_type, pos_temp++);
    //--------------------------------------
    buffer_add_string((uint8_t *)"L\x00\x03", pos_temp, 3); // Time when relay was ON
    pos_temp += 3;
    relay_time_ours    = (uint16_t)((relay_operating_time + relay_operating_time_tmp) / 60);
    relay_time_minutes =  (uint8_t)((relay_operating_time + relay_operating_time_tmp) % 60);
    buffer_add_int(relay_time_ours, pos_temp);
    pos_temp += 2;
    buffer_add_char(relay_time_minutes, pos_temp++);
    //--------------------------------------
    buffer_add_string((uint8_t *)"D\x00\x0A", pos_temp, 3); // Time stamp of thermostat
    pos_temp += 3;
    //RTC_time_get();
    buffer_add_string(timestamp_ascii, pos_temp, 10);
    pos_temp += 10;
    //--------------------------------------
    buffer_add_string((uint8_t *)"W\x00\x01", pos_temp, 3); // RSSI volume
    pos_temp += 3;
    buffer_add_char(WIFI_Level, pos_temp++);
    //--------------------------------------
    buffer_add_string((uint8_t *)"H\x00\x01", pos_temp, 3); // Relay status
    pos_temp += 3;
    buffer_add_char(relay_flag, pos_temp++);
    //--------------------------------------
    buffer_add_int(pos_temp - 6 - pos, pos + 4); // Parsel length
    //--------------------------------------
    uint16_t lenght_temp = pos_temp - pos;
    buffer_add_int(CRC16_2(Buffer, pos, lenght_temp), pos_temp); // Write CRC
    pos_temp += 2;
    //for(uint8_t y=0;y<pos_temp;y++)
    //printf("%02X",Buffer[y]);
    //printf("\n");
    return lenght_temp + 2; // Length
}

uint8_t EX_Save_Config(uint8_t *buf, uint16_t count, uint16_t size)
{
    uint8_t EE_error_flag = 1;
    uint16_t length, i, j;
    uint16_t finish;
    finish = count + size;
    //printf("%d %d\n",count,finish);
    nvs_open("storage", NVS_READWRITE, &my_handle);

    while (count < (finish))
    {

        //----------------------------------------------------------------------------------------- SERVICE COMMANDS

        if (buf[count] == 'R') // RESTART ESP
        {
            relay_operating_time += relay_operating_time_tmp;
            printf("relay_operating_time %d\n", relay_operating_time);
            nvs_open("storage", NVS_READWRITE, &my_handle);
            nvs_set_u32(my_handle, "Time_relay", relay_operating_time);
            nvs_set_u8(my_handle, "relay_flag", relay_flag);
            nvs_commit(my_handle);
            nvs_close(my_handle);

            uint8_t Position = 0;
            buffer_add_string((uint8_t *)"\x31\x00", 0, 2); // кладем инструкцию в буфер + длина пакета 6 БАЙТ
            Position += 2;
            buffer_add_int(lenght, Position); // topic length
            Position += 2;
            buffer_add_string(&BufTmp[0], Position, lenght); // topic
            Position += (lenght);
            buffer_add_string((uint8_t *)"\x00", Position, sizeof("\x00") - 1);
            Position += sizeof("\x00") - 1;
            MQTT_Encoding_size(MQTT_Encod_size, Position - 2); //func encode
            buffer_add_char(MQTT_Encod_size[0], 1);            //encode size of parsel into 2 byte
            write(s, &Buffer[0], Position);
            close(s);
            esp_restart();
        }

        if (buf[count] == 'F')
        {
            force_update = pdTRUE;
            xTaskCreate(&ota_example_task, "ota_example_task", 4096, NULL, 20, &ota_handle);
        }

        //-----------------------------------------------------------------------------------------

        if (buf[count] == 'U') // UID Домовладения
        {
            //printf("UID\n");
            count++;
            length = (buf[count] << 8);
            length |= (buf[count + 1]);
            count += 2;
            if ((strncmp((char *)&UID[0], (char *)&buf[count], length)))
            { // Сравниваем. Если разное, то пишем в память
                bzero(UID, sizeof(UID));
                memcpy((void *)&UID[0], (void *)&buf[count], length);
                nvs_set_str(my_handle, "UID", (const char *)UID);
            }

            count += length;
        }

        else if (buf[count] == 'S') // настройки
        {
            count += 3;
            if (device_power_status != (buf[count]))
            {
                if ((buf[count] == 0))
                {
                    device_power_status = buf[count];
                    nvs_set_u8(my_handle, "ON_OFF", device_power_status);

                    relay_operating_time += relay_operating_time_tmp;
                    nvs_set_u32(my_handle, "Time_relay", relay_operating_time);
                    relay_operating_time_tmp = 0;

                    bkl_led_off_timer = 0;
                    device_status = DEVICE_OFF;
                    vTaskSuspend(xTask_read_temp_handle);
                    LED_OFF;
                    RELAY_OFF;
                    relay_flag = 0;
                }
                else if (buf[count] == 1)
                {
                    device_power_status = buf[count];
                    nvs_set_u8(my_handle, "ON_OFF", device_power_status);

                    bkl_led_off_timer = BKL_OFF_TIME;
                    device_status = DEVICE_SCREEN_LOCK;
                    vTaskResume(xTask_read_temp_handle);
                    RELAY_OFF;
                    relay_flag = 0;
                }
                else
                {
                    EE_error_flag = pdFALSE;
                }
            }
            count++;

            if (heat_mode != buf[count])
            {
                if (buf[count] < 3)
                {
                    heat_mode = buf[count];
                    nvs_set_u8(my_handle, "heat_mode", heat_mode);
                }
                else
                {
                    EE_error_flag = pdFALSE;
                }
            }
            count++;

			if (sensor_set != (buf[count]))
			{
                if((buf[count] != 0) && (buf[count] <= 0x07))
                {
                    sensor_set = buf[count];
                    nvs_set_u8(my_handle, "sensor_set", sensor_set);
                }
                else
                {
                    EE_error_flag = pdFALSE;
                }
			}
			count++;

/*  			if (week_byte != (buf[count]))
			{
				week_byte = buf[count];
                byte_to_array();
                size_t work_weekend_len  = 7;
				nvs_set_blob(my_handle,  "work_weekend", (const void *)work_weekend, work_weekend_len);
			}
			count++; */

            if (set_manual_floor_temperature != buf[count])
            {
                if ((buf[count] >= 5) && (buf[count] <= 45))
                {
                    set_manual_floor_temperature = buf[count];

                    nvs_set_u8(my_handle, "set_floor_temp", set_manual_floor_temperature);
                }
                else
                {
                    EE_error_flag = pdFALSE;
                }
            }
            count++;

            if (set_air_temperature != buf[count])
            {
                if ((buf[count] >= 5) && (buf[count] <= 45))
                {
                    set_air_temperature = buf[count];

                    nvs_set_u8(my_handle, "set_air_temp", set_air_temperature);
                }
                else
                {
                    EE_error_flag = pdFALSE;
                }
            }
            count++;

/*             if (set_antifreeze_floor_temperature != buf[count])
            {
                if ((buf[count] >= 5) && (buf[count] <= 45))
                {
                    set_antifreeze_floor_temperature = buf[count];

                    nvs_set_u8(my_handle, "set_af_temp", set_antifreeze_floor_temperature);
                }
                else
                {
                    EE_error_flag = pdFALSE;
                }
            }
            count++; */

            if (((-5) <= ((int8_t)buf[count])) && (((int8_t)buf[count]) <= 5))
            {
                if ((delta_air_temperature) != (buf[count]) + 5)
                {
                    delta_air_temperature = ((int8_t)buf[count] + 5);
                    nvs_set_u8(my_handle, "delta_air_temp", delta_air_temperature);
                }
            }
            else
            {
                EE_error_flag = pdFALSE;
            }
            count++;
        }

        else if (buf[count] == 'C') // график обогрева
        {
            event_cell Temp_prog[7][4];
            count += 3;
            for (i = 0; i < 7; i++)
                for (j = 0; j < 4; j++)
                {
                    Temp_prog[i][j].event_time = ((uint16_t)buf[count++] << 8) & 0xFF00;
                    Temp_prog[i][j].event_time |= ((uint16_t)buf[count++] & 0x00FF);
                    if (Temp_prog[i][j].event_time > 1440)
                    {
                        EE_error_flag = pdFALSE;
                    }
                    Temp_prog[i][j].temperature = buf[count++];
                    if ((Temp_prog[i][j].temperature > 46) || (Temp_prog[i][j].temperature < 4))
                    {
                        EE_error_flag = pdFALSE;
                    }
                }

            if (EE_error_flag == pdTRUE)
            {
                for (i = 0; i < 7; i++)
                    for (j = 0; j < 4; j++)
                    {
                        if ((heat_schedule[i][j].temperature != Temp_prog[i][j].temperature) ||
                            (heat_schedule[i][j].event_time != Temp_prog[i][j].event_time))
                        {
                            heat_schedule[i][j] = Temp_prog[i][j];
                            //printf("Time_prog %d Temp_set %d // ",heat_schedule[i][j].event_time, heat_schedule[i][j].temperature);
                        }
                       // printf("\n");
                    }

                size_t heat_schedule_len = 112;
                nvs_set_blob(my_handle, "heat_schedule", (const void *)heat_schedule, heat_schedule_len);
                EVENT();
                set_floor_temperature_prog = heat_schedule[Day_Event_Now][Event_now].temperature;
            }
        }

        else if (buf[count] == 'D') // дата и время
        {
            uint8_t time_ascii[19];
            count += 3;
            for (i = 0; i < 19; ++i)
            {
                time_ascii[i] = buf[count++];
            }
                
            count += 2; // пропускаем резерв для часового пояса
            RTC_time_set_ascii(time_ascii);

        }

        else if (buf[count] == 'W') // Имя сети
        {
            count++;
            length = (buf[count] << 8);
            length |= (buf[count + 1]);
            count += 2;
            if (strncmp((char *)&buf[count], (char *)SSID, length))
            {
                bzero(SSID, sizeof(uint8_t) * 32);

                for (i = 0; i < length; i++)
                {
                    SSID[i] = buf[count + i];
                }
                nvs_set_str(my_handle, "SSID", (const char *)SSID);

                first_link = 1;

                nvs_set_u8(my_handle, "first_link", first_link);
            }
            reconnect_flag = 1;
            count += length;
        }
        else if (buf[count] == 'P') // Пароль к сети
        {
            count++;
            length = (buf[count] << 8);
            length |= (buf[count + 1]);
            count += 2;
            if (strncmp((char *)&buf[count], (char *)PASS, length))
            {
                bzero(PASS, sizeof(PASS));

                for (i = 0; i < length; i++)
                {
                    PASS[i] = buf[count + i];
                }
                nvs_set_str(my_handle, "PASS", (const char *)PASS);

                first_link = 1;
                nvs_set_u8(my_handle, "first_link", first_link);
            }
            reconnect_flag = 1;
            count += length;
        }
        /*
    else if(buf[count] == 'R')  // Законектиться к указанной сети
    {
      count += 3;
      FLASH_Unlock(FLASH_MemType_Data);
      First_link=1;
      AP_STA_MODE = 1;
      FLASH_Lock(FLASH_MemType_Data);
      reconnect_flag=0;
    }
*/
        else if (buf[count] == 'Z') //Тип датчика 02545157000C5300050155001F055A00020267E6
        {
            count += 3;
            if (sensor_type != (buf[count]))
            {
                if((sensor_type > 0) && (sensor_type < 7))
                {
                    sensor_type = (buf[count]);
                    nvs_set_u8(my_handle, "sensor_type", sensor_type);
                }
                else
                {
                    EE_error_flag = pdFALSE;
                }
            }
            count++;
        }

        else if (buf[count] == 'M') // Адрес и порт сервера MQTT
        {
            count++;
            length = (buf[count] << 8);
            length |= (buf[count + 1]);
            count += 2;
            uint8_t length_m = 0;
            for (uint8_t k = 0; k < length; k++)
            {
                if (buf[count + k] == ':')
                    break;
                length_m++;
            }
            if (strncmp((char *)&buf[count], (char *)MQTT_IP, length_m))
            {
                bzero(MQTT_IP, sizeof(MQTT_IP));
                for (i = 0; i < length_m; i++)
                {
                    MQTT_IP[i] = buf[count + i];
                }
                nvs_set_str(my_handle, "MQTT_IP", MQTT_IP);
            }
            count += (length_m + 1);
            length = length - length_m - 1;

            memcpy((char *)&MQTT_Port[0], (char *)&buf[count], 4);

            nvs_set_str(my_handle, "MQTT_Port", MQTT_Port);
            count += length;
        }
        else // неизвестная команда
        {
            count++; // Переходим к разбору следующего байта
        }
    }
    nvs_commit(my_handle);
    nvs_close(my_handle);

    return EE_error_flag;
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------Function send from ESP32 full buff--------------------------
//---------------------------------------------------------------------------------------------------------------
void EX_Instructions_Read(struct netconn *conn)
{
    uint16_t pos = 0;
    pos = EX_Send_Config(0);
    netconn_write(conn, (const void *)Buffer, pos, NETCONN_NOFLAG);
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------Function save full buff from server-------------------------
//---------------------------------------------------------------------------------------------------------------
void EX_Instructions_Write(struct netconn *conn, uint8_t *buf, uint16_t count, uint16_t size)
{

    uint8_t EE_error_flag;
    EE_error_flag = EX_Save_Config(buf, count, size);
    //printf("EE_error_flag %d\n",EE_error_flag);
    if (EE_error_flag == 1)
    {
        EX_response(OKEY, conn);
        if(device_power_status)
        {
            RTC_task();
            lcd_redraw();
        }
        else
        {
            clear_LCD();
        }

        //printf("reconnect_flag %d\n",reconnect_flag);
        if (reconnect_flag)
        {
            reconnect_wifi();
            reconnect_flag = 0;
        }
    }
    else
        EX_response(ERR, conn);
}

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------Function for ERR sending------------------------------------
//---------------------------------------------------------------------------------------------------------------
void EX_response(EX_Response_TypeDef response, struct netconn *conn)
{
    switch (response)
    {
    case OKEY:
        buffer_add_string((uint8_t *)"\x02\x54\x41\x00\x00\x00\x01\x4C", 0, 8);
        break;
    case ERR:
        buffer_add_string((uint8_t *)"\x02\x54\x41\xFF\x00\x00\xCE\x2F", 0, 8);
        break;
    case ECRC:
        buffer_add_string((uint8_t *)"\x02\x54\x41\xFE\x00\x00\xF9\x1F", 0, 8);
        break;
    case TIMEOUT:
        buffer_add_string((uint8_t *)"\x02\x54\x41\xFD\x00\x00\xA0\x4F", 0, 8);
        break;
    case NOACCESS:
        buffer_add_string((uint8_t *)"\x02\x54\x41\xFC\x00\x00\x97\x7F", 0, 8);
        break;
    case UNKNOW:
        buffer_add_string((uint8_t *)"\x02\x54\x41\xFF\x00\x00\xCE\x2F", 0, 8);
        break;
    }
    netconn_write(conn, (const void *)Buffer, 8, NETCONN_NOFLAG);
}

//---------------------------------------------------------------------------------------------------------------
//-----------------------------------------------Calculating CRC16-----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
uint16_t CRC16(uint8_t *DATA, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint8_t i;
    while (length--)
    {
        crc ^= *DATA++ << 8;
        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

// Расчет контрольной суммы не с начала пакета
uint16_t CRC16_2(uint8_t *DATA, uint16_t start, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint8_t i;
    uint16_t v_ind = start;
    while (v_ind < start + length)
    {
        crc ^= DATA[v_ind] << 8;
        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
        v_ind++;
    }
    return crc;
}


//---------------------------------------------------------------------------------------------------------------
//----------------------------------------------TCP Server task--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

void tcp_server(void *ptr)
{
	struct netconn *tcp_conn = netconn_new(NETCONN_TCP);
	if (tcp_conn)
	{
		netconn_bind(tcp_conn, NULL, 6350);
		netconn_listen(tcp_conn);
		printf("TCP Server listening...\n");
	}
	Access_flag = 0;

    uint32_t ip_first   = 0;
    uint32_t ip_last    = 0;
	uint16_t des_Port   = 0;

	struct netconn *newconn;

	while (1)
	{
		//tcp_conn->recv_timeout = 1000;
		//printf("accepting\n");
		netconn_accept(tcp_conn, &newconn);

		netconn_getaddr(newconn, (ip_addr_t *)&ip_first, &des_Port, 0);

		if (ip_last == 0)
		{
			ip_last = (uint32_t)ip_first;
			//conditions_register = (conditions_register | CONN);
		}
		if (ip_last == ip_first)
		{
			//conn_Counter = 0;
		}
		else
		{
			Access_flag = 1;
		}

		//---------------------------------------------------------------------------------------------------

		tcp_server_service(newconn);
		//vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------------------------------------TCP management---------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
void tcp_server_service(struct netconn *conn)
{

	struct T_instruction instruction;

	struct netbuf *inbuf;

	uint8_t *buf;

	uint16_t CRC;
	uint16_t buflen = 0;
	uint16_t count = 0;
	err_t err;

	err = netconn_recv(conn, &inbuf);

	if ((err == ERR_OK))
	{

		netbuf_data(inbuf, (void **)&buf, &buflen);
		if (buflen)
		{

			/* for(int i=0;i<buflen;i++)
			{
				printf("%02X",buf[i]);
			}
			printf("\n"); */

			if ((buf[count] == 0x02) && (buf[count + 1] == 'T') && (buf[count + 2] == 'Q'))
			{

				count += 3;
				instruction.ID = buf[count++];

				instruction.length = ((uint16_t)(buf[count] << 8) & 0xFF00);
				instruction.length |= ((uint16_t)buf[count + 1]) & 0x00FF;
				count += 2;
				instruction.CRC = (((uint16_t)buf[count + instruction.length] << 8) & 0xFF00);
				instruction.CRC |= ((uint16_t)buf[count + instruction.length + 1]) & 0x00FF;
				CRC = CRC16(buf, buflen - 2);
				if (instruction.CRC == CRC)
				{
					if (instruction.ID == 'R')
					{
						EX_Instructions_Read(conn);
					}
					else if ((instruction.ID == 'W'))
					{
						EX_Instructions_Write(conn, buf, count, instruction.length); // - 2B crc
					}
					else
						printf("Access\n");
				}
				else
				{
					EX_response(ECRC, conn);
					printf("false crc\n");
				}
			}
			else
				EX_response(UNKNOW, conn);
		}
		netbuf_free(inbuf);
		netconn_close(conn);
	}
	else
		printf("Resv err %d\n", err);
	//}
}

//---------------------------------------------------------------------------------------------------------------
//----------------------------------------------UDP server task--------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
void udp_server(void *ptr)
{

	struct netconn *udp_conn;
	udp_conn = netconn_new(NETCONN_UDP);
	netconn_bind(udp_conn, IP_ADDR_ANY, 6350);
	printf("UDP Server listening...\n");
	while (1)
	{
		//printf("UDP\n");
		udp_server_service(udp_conn);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

//---------------------------------------------------------------------------------------------------------------
//----------------------------------------------UDP management---------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
void udp_server_service(struct netconn *conn)
{

	struct netbuf *inbuf;

	char *buf;
	uint16_t buflen;
	err_t err;

	err = netconn_recv(conn, &inbuf);

	if (err == ERR_OK)
	{

		netbuf_data(inbuf, (void **)&buf, &buflen);

		/*
		for(int i=0;i<buflen;i++)
		{
			printf("%02X",buf[i]);
		}
		printf("\n");
*/
		if (!strncmp((char *)&buf[0], "\x02TQI\x00\x00\x99\xD7", 8))
		{
			buffer_add_string((uint8_t *)"\x02TAI\x00\x16", 0, 6);
                //buffer_add_string((uint8_t *)"T4", 6, 2);
			buffer_add_string((uint8_t *)"E1", 6, 2);
			buffer_add_string((uint8_t *)VERSION, 8, 3);
			buffer_add_string((uint8_t *)MAC_esp, 11, 17);
			buffer_add_int(CRC16(Buffer, 28), 28);
			/*
			for(int i=0;i<30;i++)
			{
				printf("%02X",Buffer[i]);
			}
			printf("\n");
			*/
			netbuf_ref(inbuf, (void *)&Buffer, 30);
			netconn_send(conn, inbuf);
		}
	}
	netbuf_delete(inbuf);
}

//=============================================================================================================================================
//================================================================= MQTT ======================================================================
//=============================================================================================================================================
int s; //socket pointer

bool Connect_mqttserver()
{

    struct addrinfo *res;
    const struct addrinfo hints =
        {
            .ai_family = AF_INET,
            .ai_socktype = SOCK_STREAM,
        };

    //printf("MQTT_Port %s\n", MQTT_Port);
    // resolve the IP of the target website
    int result = getaddrinfo(&MQTT_IP[0], &MQTT_Port[0], &hints, &res);
    if ((result != 0) || (res == NULL))
    {
        printf("Unable to resolve IP for target website %s\n", &MQTT_IP[0]);
        return pdFALSE;
    }
    //printf("Target website's IP resolved\n");

    // create a new socket
    s = socket(res->ai_family, res->ai_socktype, 0);
    if (s < 0)
    {
        printf("Unable to allocate a new socket\n");
        freeaddrinfo(res);
        return pdFALSE;
    }
    //printf("Socket allocated, id=%d\n", s);

    // connect to the specified server
    result = connect(s, res->ai_addr, res->ai_addrlen);
    if (result != 0)
    {
        printf("Unable to connect to the target website\n");
        freeaddrinfo(res);
        return pdFALSE;
    }
    //printf("Connected to the target website\n");
    freeaddrinfo(res);
    bzero(Buffer, sizeof(Buffer));
    bzero(recv_buf, sizeof(recv_buf));

    buffer_add_string((uint8_t *)"\x10\x1D\x00\x04MQTT\x04\x02\x00\x00\x00\x11", 0, 14);
    buffer_add_string((uint8_t *)MAC_esp, 14, 17);

#ifdef DEBUG
    printf("\n");
#endif
    result = write(s, Buffer, 31);
    //printf("%d\n", result);
    if (result < 0)
    {
        printf("Unable to send the HTTP request\n");
        //close(s);
        return pdFALSE;
    }

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0)
    {
        printf("... failed to set socket receiving timeout\n");
        close(s);
        return pdFALSE;
    }
    //printf("... set socket receiving timeout success\n");

    //---------------------------------------------------------------------------------------------- receive
    uint16_t num_bytes = 0;
    num_bytes = read(s, &recv_buf[0], sizeof(recv_buf) - 1);
    //printf("num_bytes %d\n", num_bytes);

    if (num_bytes == 0)
    {
        //close(s);
        return pdFALSE;
    }

#ifdef DEBUG
    printf("\n");
#endif

    if ((recv_buf[0] == 0x20) && (recv_buf[1] == 0x02))
    {
        return pdTRUE;
    }
    else
    {
        //close(s);
        return pdFALSE;
    }

    //----------------------------------------------------------------------------------------------
}

void mqtt_subscribe(void *ptr)
{

    //vTaskDelay(5000 / portTICK_PERIOD_MS);
    while (1)
    {
        uint16_t count = 0;
        uint8_t ID;
        uint16_t CRC, CRC_Load;

        if (Connect_mqttserver())
        {
            uint16_t Position = 0;

            uint16_t num_bytes;

            bzero(Buffer, sizeof(Buffer));
            bzero(BufTmp, sizeof(BufTmp));

            buffer_add_string((uint8_t *)&UID[0], Position, sizeof(UID) - 1);
            Position += sizeof(UID) - 1;
            buffer_add_char('/', Position++);
            buffer_add_string((uint8_t *)&MAC_esp[0], Position, sizeof(MAC_esp) - 1);
            Position += sizeof(MAC_esp) - 1;
            buffer_add_char('/', Position++);
            buffer_add_string((uint8_t *)"to", Position, sizeof("to") - 1);
            Position += sizeof("to") - 1;
            lenght = sizeof(UID) - 1 + sizeof(MAC_esp) - 1 + sizeof("to") - 1 + 2;

            memcpy((void *)BufTmp, (void *)Buffer, lenght);

            Position = 0;
            buffer_add_string((uint8_t *)"\x82\x00\x00\x01", 0, 4); // кладем инструкцию в буфер + длина пакета 6 БАЙТ
            Position += 4;
            buffer_add_int(lenght, Position); // topic length
            Position += 2;
            buffer_add_string(&BufTmp[0], Position, lenght); // topic
            Position += (lenght);
            buffer_add_char(0, Position++);                    //qos = 0
            MQTT_Encoding_size(MQTT_Encod_size, Position - 2); //func encode
            buffer_add_char(MQTT_Encod_size[0], 1);            //encode size of parsel into 2 byte

            num_bytes = 0;
            //printf("\n");

            write(s, &Buffer[0], Position);
            //printf("%d\n", result);

            bzero(recv_buf, sizeof(recv_buf));

            //vTaskDelay(1000 / portTICK_RATE_MS);
            num_bytes = read(s, &recv_buf[0], sizeof(recv_buf));

            if ((recv_buf[0] == 0x90) && (recv_buf[1] == 0x03))
            {
                bzero(recv_buf, sizeof(recv_buf));

                num_bytes = read(s, &recv_buf[0], sizeof(recv_buf));
                if (num_bytes > 500)
                {
                    Position = 0;
                    buffer_add_string((uint8_t *)"\x31\x00", 0, 2); // кладем инструкцию в буфер + длина пакета 6 БАЙТ
                    Position += 2;
                    buffer_add_int(lenght, Position); // topic length
                    Position += 2;
                    buffer_add_string(&BufTmp[0], Position, lenght); // topic
                    Position += (lenght);
                    buffer_add_string((uint8_t *)"\x00", Position, sizeof("\x00") - 1);
                    Position += sizeof("\x00") - 1;
                    MQTT_Encoding_size(MQTT_Encod_size, Position - 2); //func encode
                    buffer_add_char(MQTT_Encod_size[0], 1);            //encode size of parsel into 2 byte
                    write(s, &Buffer[0], Position);
                }
                else
                {
#ifdef DEBUG
                    printf("%d\n", num_bytes);
                    printf("SUBSCRIBE***************************************************************************************\n");
                    for (uint8_t u = lenght + 4; u < num_bytes; u++)
                    {
                        printf("%02X", recv_buf[u]);
                    }
                    printf("\n");
                    printf("************************************************************************************************\n");

                    printf("\n");
#endif
                }
                if (num_bytes && (Access_flag == 0))
                {
                    while (count < num_bytes)
                    {
                        if (!strncmp((char *)&recv_buf[count], "\x02TQW", 4))
                        {
                            count += 3;
                            ID = recv_buf[count++];
                            lenght_p = ((uint16_t)(recv_buf[count] << 8) & 0xFF00);
                            lenght_p |= ((uint16_t)recv_buf[count + 1]) & 0x00FF;
                            count += 2;
                            CRC = CRC16_2((uint8_t *)recv_buf, count - 6, lenght_p + 6);
                            CRC_Load = (((uint16_t)recv_buf[count + lenght_p] << 8) & 0xFF00);
                            CRC_Load |= ((uint16_t)recv_buf[count + lenght_p + 1]) & 0x00FF;
                            if (CRC_Load == CRC)
                            {
                                if (ID == 'W')
                                {
                                    //затираем топик после получения
                                    Position = 0;
                                    buffer_add_string((uint8_t *)"\x31\x00", 0, 2); // кладем инструкцию в буфер + длина пакета 6 БАЙТ
                                    Position += 2;
                                    buffer_add_int(lenght, Position); // topic length
                                    Position += 2;
                                    buffer_add_string(&BufTmp[0], Position, lenght); // topic
                                    Position += (lenght);
                                    buffer_add_string((uint8_t *)"\x00", Position, sizeof("\x00") - 1);
                                    Position += sizeof("\x00") - 1;
                                    MQTT_Encoding_size(MQTT_Encod_size, Position - 2); //func encode
                                    buffer_add_char(MQTT_Encod_size[0], 1);            //encode size of parsel into 2 byte
                                    write(s, &Buffer[0], Position);

                                    //Save settings
                                    EX_Save_Config((uint8_t *)&recv_buf[0], count, lenght_p);

                                    
                                    if (device_power_status)
                                    {
                                        RTC_task();
                                        lcd_redraw();
                                    }
                                    else
                                    {
                                        clear_LCD();
                                    }

                                    if (reconnect_flag)
                                    {

                                        reconnect_wifi();
                                        reconnect_flag = 0;
                                    }

                                    break;
                                }
                            }
                        }
                        count++;
                    }
                }
                else
                {
#ifdef DEBUG
                    printf("Access denied\n");
#endif
                }

                //printf("Subscribe!\n");
                //*********************************************************************************************************************
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                uint16_t Position = 0;
                uint16_t lenght = 0;

                bzero(Buffer, sizeof(Buffer));
                bzero(BufTmp, sizeof(BufTmp));

                buffer_add_string((uint8_t *)&UID[0], Position, sizeof(UID) - 1);
                Position += sizeof(UID) - 1;
                buffer_add_char('/', Position++);
                buffer_add_string((uint8_t *)&MAC_esp[0], Position, sizeof(MAC_esp) - 1);
                Position += sizeof(MAC_esp) - 1;
                buffer_add_char('/', Position++);
                buffer_add_string((uint8_t *)"from", Position, sizeof("from") - 1);
                Position += sizeof("from") - 1;
                lenght = sizeof(UID) - 1 + sizeof(MAC_esp) - 1 + sizeof("from") - 1 + 2;
                memcpy((void *)BufTmp, (void *)Buffer, lenght);

                MQTT_Encoding_size(MQTT_Encod_size, EX_Send_Config(0) + lenght + 2); //кодируем длина топика +2байта длины топика + длина конфига
                bzero(Buffer, sizeof(Buffer));
                Position = 0;
                if (MQTT_Encod_size[1] == 0x00) //если длина пакета меньше 128байт
                {
                    buffer_add_string((uint8_t *)"\x31\x00", 0, 2);
                    Position += 2;
                    buffer_add_char(MQTT_Encod_size[0], 1);
                }
                else //если больше, то добавляется 2 байт блины посылки после первого
                {
                    buffer_add_string((uint8_t *)"\x31\x00\00", 0, 3);
                    Position += 3;
                    buffer_add_string(&MQTT_Encod_size[0], 1, 2);
                }

                buffer_add_int(lenght, Position); // topic length
                Position += 2;
                buffer_add_string(&BufTmp[0], Position, lenght); // topic
                Position += (lenght);
                uint16_t pos = 0;
                pos = EX_Send_Config(Position);
                Position += pos;
                /*
				printf("PUBLISH***************************************************************************************\n");
				for (uint8_t u = 0; u < Position; u++)
				{
					printf("%02X", Buffer[u]);
				}
				printf("\n");
				printf("**********************************************************************************************\n");

*/
                write(s, &Buffer[0], Position);
                //printf("%d\n", result);
                close(s);
                //printf("Publish!\n");
                //vTaskDelete(NULL);
            }
            else
            {
                close(s);
                bzero(recv_buf, sizeof(recv_buf));
                //vTaskDelete(NULL);
            }
        }
        else
        {
            //printf("Connection false\n");
            close(s);
            //vTaskDelete(NULL);
        }
        //printf("MQTT\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        //vTaskDelete(NULL);
    }
}

void MQTT_Encoding_size(uint8_t *encoding_result, uint16_t size) // функция кодирования длины пакета
{
    uint8_t temp;
    while (size > 0)
    {
        temp = size % 128;
        size = size / 128;
        if (size)
        {
            temp |= 128;
        }
        *encoding_result = temp;
        encoding_result++;
    }
}
