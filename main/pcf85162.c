#include "device_init.h"



void send_data_to_lcd(uint8_t *data_to_lcd)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, 0x70, 1); // Device Address 0x70
	i2c_master_write_byte(cmd, 0xC8, 1);
	i2c_master_write_byte(cmd, 0x00, 1);
    i2c_master_write(cmd, data_to_lcd, LCD_BUFF_SIZE, 1);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(LCD_I2C, cmd, 10);

	i2c_cmd_link_delete(cmd);
}


void init_lcd()
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, 0x70, 1); // Device Address 0x70
	i2c_master_write_byte(cmd, 0x48, 1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(LCD_I2C, cmd, 10);

	i2c_cmd_link_delete(cmd);
}