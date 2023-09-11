#ifndef DEFINES_H_
#define DEFINES_H_

#define VERSION                 "100"

#define TOUCH_THRESH_NO_USE     (0)
#define TOUCH_THRESH_PERCENT    (94)

#define LCD_BUFF_SIZE	        16
#define BKL_OFF_TIME	        80                      // TIME FOR BACKLIGHT OFF/LOCK IN SEC*2

#define RTC_SDA_PIN	            22
#define RTC_SCL_PIN	            19
#define LCD_SDA_PIN	            23
#define LCD_SCL_PIN	            18
#define RTC_I2C	                0
#define LCD_I2C	                1

#define BEEP_GPIO	            10
#define BEEP_DELAY	            20

#define COUNT_SHORT_CLICK       1
#define COUNT_DSHORT_CLICK      35
#define COUNT_LONG_CLICK        250                     // 3 sec
#define COUNT_DLONG_CLICK       251                     

#define RELAY_ON	            gpio_set_level(4, 1)
#define RELAY_OFF	            gpio_set_level(4, 0)

#define LED_ON	                gpio_set_level(9, 1)
#define LED_OFF	                gpio_set_level(9, 0)

#define BUTTON_MINUS            TOUCH_PAD_NUM2
#define BUTTON_PLUS             TOUCH_PAD_NUM8
#define BUTTON_MODE_OK          TOUCH_PAD_NUM4
#define BUTTON_ON_OFF           TOUCH_PAD_NUM5
#define BUTTON_SET_CANCEL       TOUCH_PAD_NUM7

#endif

