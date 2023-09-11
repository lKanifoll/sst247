#ifndef PCF85063_H_
#define PCF85063_H_

#include "device_init.h"

#define L_YEAR            31536000
#define L_DAY             86400
#define L_HOUR            3600
#define L_MIN             60



typedef struct
{
    uint8_t RTC_Hours;
    uint8_t RTC_Minutes;
    uint8_t RTC_Seconds;
} RTC_TimeTypeDef;

typedef struct
{
    uint8_t RTC_WeekDay;
    uint8_t RTC_Date;
    uint8_t RTC_Month;
    uint8_t RTC_Year;

} RTC_DateTypeDef;

RTC_TimeTypeDef RTC_TimeStr;
RTC_DateTypeDef RTC_DateStr;

uint8_t pcf85063_data[7];
uint8_t timestamp_ascii[11];
uint8_t time_ascii[20];
uint8_t timezone[2];
uint8_t Event_now, Event_next, Day_Event_Now, Day_Event_Next, tmp_Event_now;
uint16_t Time_min24;
uint16_t Timer_min;
uint16_t Temp_minuts;
uint32_t timestamp;







void RTC_task();
uint8_t BCDtoBIN(uint8_t BDC);
uint8_t BINtoBCD(uint8_t BIN);
uint8_t RTC_get_week_day(uint8_t date, uint8_t month, uint16_t year);
uint32_t RTC_timestamp_get(uint8_t day, uint8_t month, uint8_t year, uint8_t hour, uint8_t min, uint8_t sec);
void reverse(char s[]);
void inttoascii(long int n, char s[]);
void EVENT(void);
void RTC_time_set_ascii(uint8_t* datetime_ascii);
void rtc_set(uint8_t sec,uint8_t min, uint8_t ours, uint8_t date, uint8_t weekday, uint8_t month, uint8_t year);
void reset_clock();

#endif
