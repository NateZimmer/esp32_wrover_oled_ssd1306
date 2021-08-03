

# ESP32 IDF SSD1306 OLED Example

Text example for 128x64 OLED display with SSD1306 driver. 

## Example

Example API: 

```c
void app_main(void)
{
	
	i2c_master_init();
	ssd1306_init();

	ssd1306_display_clear();

	vTaskDelay(1000/portTICK_PERIOD_MS);
	
	ssd1306_DrawText8x8("Hurro", 50, 2);
	ssd1306_DrawText12x16("Hurro", 0, 4);

}
```

Code is formated such that the screen lib is a component 


## Borrows from:  

https://github.com/yanbe/ssd1306-esp-idf-i2c/blob/master/main/main.c
