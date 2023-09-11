#ifndef WORKFLOW_H_
#define WORKFLOW_H_

#include "device_init.h"

uint8_t plus_minus_flag;

typedef enum
{
    DEVICE_OFF = 0,
    DEVICE_SCREEN_LOCK,
    DEVICE_SCREEN_UNLOCK,
    DEVICE_MANUAL_CONFIG,
    DEVICE_TEMP_ADJ_MANUAL,
    DEVICE_MODE_SET,

} device_status_t;

typedef enum
{
    MANUAL_MODE = 0,
    PROGRAM_MODE,
    ANTIFREEZE_MODE,
} device_mode_t;
//=============================== BUTTONS 
typedef enum
{
    NO_CLICK = 0,
    SHORT_CLICK,
    DSHORT_CLICK,
    LONG_CLICK,
    DLONG_CLICK
} TypeClick_t;

typedef struct
{
    uint8_t flag_click;
    uint16_t counter;
    TypeClick_t type_click;
    uint16_t clock_short_click;
    uint16_t clock_dshort_click;
    uint16_t clock_long_click;
    uint16_t clock_dlong_click;
} Key_Param_t;

Key_Param_t BT_PLUS_PAR;
Key_Param_t BT_MINUS_PAR;
Key_Param_t BT_MODE_OK_PAR;
Key_Param_t BT_ON_OFF_PAR;
Key_Param_t BT_SET_CANCEL_PAR;



int8_t Calculate_Temper(uint16_t Code, uint8_t Sens_type);
void device_menu(void);
uint8_t array_to_byte();
void byte_to_array();

#endif