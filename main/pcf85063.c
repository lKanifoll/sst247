#include "device_init.h"

void reset_clock()
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (0x51 << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, 0x00, 1); //start adress of date

    i2c_master_write_byte(cmd, 0x01, 1);
    i2c_master_write_byte(cmd, 0x00, 1);
    i2c_master_write_byte(cmd, 0x00, 1);
    i2c_master_write_byte(cmd, 0x00, 1);
    i2c_master_write_byte(cmd, 0x01, 1); //seconds
    i2c_master_write_byte(cmd, 0x00, 1); //minutes
    i2c_master_write_byte(cmd, 0x12, 1); //ours
    i2c_master_write_byte(cmd, 0x01, 1); //date
    i2c_master_write_byte(cmd, 0x00, 1); //weekday
    i2c_master_write_byte(cmd, 0x01, 1); //month
    i2c_master_write_byte(cmd, 0x18, 1); //year
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(RTC_I2C, cmd, 10);

    i2c_cmd_link_delete(cmd);
}

//==========================================================================================
//============================================================================= Task for RTC
//==========================================================================================
void RTC_task()
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (0x51 << 1) | I2C_MASTER_WRITE, 1);

    i2c_master_write_byte(cmd, 0x04, 1); 

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (0x51 << 1) | I2C_MASTER_READ, 1);

    i2c_master_read_byte(cmd, pcf85063_data, 0);
    i2c_master_read_byte(cmd, pcf85063_data + 1, 0);
    i2c_master_read_byte(cmd, pcf85063_data + 2, 0);
    i2c_master_read_byte(cmd, pcf85063_data + 3, 0);
    i2c_master_read_byte(cmd, pcf85063_data + 4, 0);
    i2c_master_read_byte(cmd, pcf85063_data + 5, 0);
    i2c_master_read_byte(cmd, pcf85063_data + 6, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(RTC_I2C, cmd, 1);

    RTC_TimeStr.RTC_Seconds = BCDtoBIN(pcf85063_data[0]);
    RTC_TimeStr.RTC_Minutes = BCDtoBIN(pcf85063_data[1]);
    RTC_TimeStr.RTC_Hours = BCDtoBIN(pcf85063_data[2]);
    //RTC_DateStr.RTC_WeekDay = BCDtoBIN(pcf85063_data[4]);
    RTC_DateStr.RTC_Date = BCDtoBIN(pcf85063_data[3]);
    RTC_DateStr.RTC_Month = BCDtoBIN(pcf85063_data[5]);
    RTC_DateStr.RTC_Year = BCDtoBIN(pcf85063_data[6]);
    RTC_DateStr.RTC_WeekDay = RTC_get_week_day(RTC_DateStr.RTC_Date, RTC_DateStr.RTC_Month, RTC_DateStr.RTC_Year);

    
/*   	printf("--------------\n");
	printf("Time %d:", RTC_TimeStr.RTC_Hours);
	printf("%d:", RTC_TimeStr.RTC_Minutes);
	printf("%d\n", RTC_TimeStr.RTC_Seconds);
	printf("Date %d", RTC_DateStr.RTC_Date);
	printf("/%d", RTC_DateStr.RTC_Month);
	printf("/%d\n", RTC_DateStr.RTC_Year);
	printf("--------------\n"); */

    timestamp = RTC_timestamp_get(RTC_DateStr.RTC_Date, RTC_DateStr.RTC_Month, RTC_DateStr.RTC_Year, RTC_TimeStr.RTC_Hours, RTC_TimeStr.RTC_Minutes, RTC_TimeStr.RTC_Seconds);
    //printf("timestamp %d\n",timestamp);
    inttoascii(timestamp, (char *)timestamp_ascii);
    //printf("RTC_WeekDay %d\n",RTC_DateStr.RTC_WeekDay);

    i2c_cmd_link_delete(cmd);

    Time_min24=(unsigned int)RTC_TimeStr.RTC_Minutes+(unsigned int)RTC_TimeStr.RTC_Hours*60;
    EVENT();
    //printf("Day_Event_Now: %d, Event_now: %d\n", Day_Event_Now, Event_now);
    //printf("Time_min24 %d, heat_shed: %d \n", Time_min24, heat_schedule[Day_Event_Now][Event_now].event_time);
    if((device_status == DEVICE_SCREEN_LOCK))
	{
		lcd_redraw();
	}
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
    if (RTC_DateStr.RTC_Year < 18)
    {
        reset_clock();
    }
}



void rtc_set(uint8_t sec, uint8_t min, uint8_t ours, uint8_t date, uint8_t weekday, uint8_t month, uint8_t year)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (0x51 << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, 0x00, 1); //start adress of date

    i2c_master_write_byte(cmd, 0x01, 1);
    i2c_master_write_byte(cmd, 0x00, 1);
    i2c_master_write_byte(cmd, 0x00, 1);
    i2c_master_write_byte(cmd, 0x00, 1);
    i2c_master_write_byte(cmd, sec, 1);     //seconds
    i2c_master_write_byte(cmd, min, 1);     //minutes
    i2c_master_write_byte(cmd, ours, 1);    //ours
    i2c_master_write_byte(cmd, date, 1);    //date
    i2c_master_write_byte(cmd, weekday, 1); //weekday
    i2c_master_write_byte(cmd, month, 1);   //month
    i2c_master_write_byte(cmd, year, 1);    //year
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(RTC_I2C, cmd, 10);
    i2c_cmd_link_delete(cmd);
}

uint8_t BCDtoBIN(uint8_t BDC)
{
    return (BDC & 0x0F) + (((BDC & 0x70) >> 4) * 10);
}

uint8_t BINtoBCD(uint8_t BIN)
{
    return ((BIN / 10) << 4) + BIN % 10;
}

uint32_t RTC_timestamp_get(uint8_t day, uint8_t month, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t timestamp = 946684800;
    uint8_t monthes[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t i;
    timestamp += year * L_YEAR + ((year - 1) / 4 + 1) * L_DAY;
    for (i = 0; i < (month - 1); ++i)
        timestamp += monthes[i] * L_DAY;

    timestamp += (day - 1) * L_DAY;
    if ((year % 4 == 0) & (month > 2))
        timestamp += L_DAY;

    timestamp += (hour) * (uint32_t)L_HOUR;
    timestamp += (min)*L_MIN;
    timestamp += sec;
    return timestamp;
}

void inttoascii(long int n, char s[])
{
    long int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do
    {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

uint8_t RTC_get_week_day(uint8_t date, uint8_t month, uint16_t year)
{
    uint8_t a = (14 - month) / 12;
    uint16_t y = year - a;
    uint16_t m = month + 12 * a - 2;
    return (((date + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) - 1) % 7);
}

void RTC_time_set_ascii(uint8_t *datetime_ascii) // 25/03/2013,14:37:45
{
    uint8_t tmp[3] = {0};
    uint8_t tmp4[5] = {0};
    uint8_t mday;
    uint8_t month;
    uint8_t year;
    uint8_t wday;
    uint8_t ours;
    uint8_t min;
    uint8_t sec;

    tmp[0] = datetime_ascii[0];
    tmp[1] = datetime_ascii[1];
    mday = atoi((char *)tmp);

    tmp[0] = datetime_ascii[3];
    tmp[1] = datetime_ascii[4];
    month = atoi((char *)tmp);

    tmp4[0] = datetime_ascii[6];
    tmp4[1] = datetime_ascii[7];
    tmp4[2] = datetime_ascii[8];
    tmp4[3] = datetime_ascii[9];

    year = atoi((char *)tmp4) - 2000;

    wday = RTC_get_week_day(mday, month, year);
    //printf("***********************wday %d\n",wday);
    tmp[0] = datetime_ascii[11];
    tmp[1] = datetime_ascii[12];
    ours = atoi((char *)tmp);

    tmp[0] = datetime_ascii[14];
    tmp[1] = datetime_ascii[15];
    min = atoi((char *)tmp);

    tmp[0] = datetime_ascii[17];
    tmp[1] = datetime_ascii[18];
    sec = atoi((char *)tmp);

    rtc_set(BINtoBCD(sec), BINtoBCD(min), BINtoBCD(ours), BINtoBCD(mday), wday, BINtoBCD(month), BINtoBCD(year));
}

void EVENT(void)
{
    uint8_t i = 0;
    while (i < 3)
    {
        Event_now = 3;
        Event_next = 0;

        if (((Time_min24 >= heat_schedule[RTC_DateStr.RTC_WeekDay][i].event_time) && (Time_min24 < heat_schedule[RTC_DateStr.RTC_WeekDay][i + 1].event_time)))
        {
            Event_now = i;
            if (i < 3)
                Event_next = i + 1;
            Day_Event_Now = RTC_DateStr.RTC_WeekDay;
            Day_Event_Next = RTC_DateStr.RTC_WeekDay;

            break;
        }
        i++;
    }
    if (!Event_next)
    {
        if (((RTC_DateStr.RTC_WeekDay == 0) && ((Time_min24 < heat_schedule[0][0].event_time))) || ((RTC_DateStr.RTC_WeekDay == 6) && ((Time_min24 <= 1440) && (Time_min24 > heat_schedule[6][3].event_time))))
        {
            Day_Event_Now = 6;
            Day_Event_Next = 0;
        }
        else
        {
            if ((Time_min24 < heat_schedule[RTC_DateStr.RTC_WeekDay][0].event_time))
            {
                Day_Event_Now = RTC_DateStr.RTC_WeekDay - 1;
                Day_Event_Next = Day_Event_Now + 1;
            }
            else
            {
                Day_Event_Now = RTC_DateStr.RTC_WeekDay;
                Day_Event_Next = Day_Event_Now + 1;
            }
        }
    }
};
