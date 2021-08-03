
#include "lcd_lib_ssd1306.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
	
	i2c_master_init();
	ssd1306_init();

	ssd1306_display_clear();

	vTaskDelay(1000/portTICK_PERIOD_MS);
	
	ssd1306_DrawText8x8("Hurro", 50, 2);
	// ssd1306_DrawText6x8("Hurro", 30, 4);
	ssd1306_DrawText12x16("Hurro", 0, 4);
	

}
