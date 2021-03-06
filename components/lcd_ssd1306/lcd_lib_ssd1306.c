#include <string.h>

#include "lcd_lib_ssd1306.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h" // generated by "make menuconfig"

#include "ssd1366.h"
#include "lcd_fonts.h"

#define SDA_PIN GPIO_NUM_18
#define SCL_PIN GPIO_NUM_2

#define tag "SSD1306"

void i2c_master_init()
{
	i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = SDA_PIN,
		.scl_io_num = SCL_PIN,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 100000
	};
	i2c_param_config(I2C_NUM_0, &i2c_config);
	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void ssd1306_init() {
	esp_err_t espRc;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);

	i2c_master_write_byte(cmd, OLED_CMD_SET_CHARGE_PUMP, true);
	i2c_master_write_byte(cmd, 0x14, true);

	i2c_master_write_byte(cmd, OLED_CMD_SET_SEGMENT_REMAP, true); // reverse left-right mapping
	i2c_master_write_byte(cmd, OLED_CMD_SET_COM_SCAN_MODE, true); // reverse up-bottom mapping

	i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_ON, true);
	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	if (espRc == ESP_OK) {
		ESP_LOGI(tag, "OLED configured successfully");
	} else {
		ESP_LOGE(tag, "OLED configuration failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);
}

void ssd1306_display_clear() {
	i2c_cmd_handle_t cmd;

	uint8_t zero[128] = {0};
	for (uint8_t i = 0; i < 8; i++) {
		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_SINGLE, true);
		i2c_master_write_byte(cmd, 0xB0 | i, true);

		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
		i2c_master_write(cmd, zero, 128, true);
		i2c_master_stop(cmd);
		i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd);
	}
}

void ssd1306_SetPagePos(uint8_t xPixel, uint8_t yPage)
{
	i2c_cmd_handle_t cmd;

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_CMD_STREAM, true);
	
	i2c_master_write_byte(cmd, 0x00 | (xPixel & 0x0F) , true); // Set lower column bits
	
	i2c_master_write_byte(cmd, 0x10 | (xPixel >> 4), true); // Set upper column bits 
	
	i2c_master_write_byte(cmd, 0xB0 | yPage, true); // reset page

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);	
}


void ssd1306_DrawChar12x16(char ch, uint8_t xPixel, uint8_t yPage)
{
	i2c_cmd_handle_t cmd;
	uint8_t c = (uint8_t)ch - 32;
	uint8_t temp = 0;
	ssd1306_SetPagePos(xPixel, yPage);
	uint8_t cBuf[12] = {0};

	for(uint8_t i = 0 ; i < 6; i++)
	{
		temp = font6x8[c * 6 + i];
		for(uint8_t j = 0; j < 8; j++)
		{
			cBuf[i*2] |= temp & (1 << (3 - (j/2))) ? (1 << (7-j)) : 0;
		}
		cBuf[i*2 + 1] = cBuf[i*2];
	}	

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
	i2c_master_write(cmd, cBuf, 12, true);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	memset(cBuf,0,sizeof(cBuf));

	for(uint8_t i = 0 ; i < 6; i++)
	{
		temp = font6x8[c * 6 + i];
		for(uint8_t j = 0; j < 8; j++)
		{
			cBuf[i*2] |= temp & (1 << (7 - (j/2))) ? (1 << (7-j)) : 0;
		}
		cBuf[i*2 + 1] = cBuf[i*2];
	}

	ssd1306_SetPagePos(xPixel, yPage+1);

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
	i2c_master_write(cmd, cBuf, 12, true);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

}

void ssd1306_DrawText12x16(char * str, uint8_t xPixel, uint8_t yPage)
{
	uint8_t text_len = strlen(str);
		
	for (uint8_t i = 0; i < text_len; i++) {
		ssd1306_DrawChar12x16(str[i], xPixel + i*12, yPage);
	}
}

void ssd1306_DrawText6x8(char * str, uint8_t xPixel, uint8_t yPage)
{
	uint8_t text_len = strlen(str);
	i2c_cmd_handle_t cmd;
	uint8_t c = 0;
	ssd1306_SetPagePos(xPixel, yPage);

	for (uint8_t i = 0; i < text_len; i++) {
		c = (uint8_t)str[i] - 32;
		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
		i2c_master_write(cmd, &font6x8[c * 6], 6, true);

		i2c_master_stop(cmd);
		i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd);
	}
}

void ssd1306_DrawText8x8(char * str, uint8_t xPixel, uint8_t yPage)
{
	uint8_t text_len = strlen(str);
	i2c_cmd_handle_t cmd;

	ssd1306_SetPagePos(xPixel, yPage);

	for (uint8_t i = 0; i < text_len; i++) {
		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

		i2c_master_write_byte(cmd, OLED_CONTROL_BYTE_DATA_STREAM, true);
		i2c_master_write(cmd, font8x8_basic_tr[(uint8_t)str[i]], 8, true);

		i2c_master_stop(cmd);
		i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd);
	}
}

