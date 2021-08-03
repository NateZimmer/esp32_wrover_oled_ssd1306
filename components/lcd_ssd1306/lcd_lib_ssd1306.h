
#include <stdint.h>

#ifndef LCD_LIB_SSD1306_H
#define LCD_LIB_SSD1306_H

void i2c_master_init();
void ssd1306_init();
void ssd1306_display_clear();
void ssd1306_SetPagePos(uint8_t xPixel, uint8_t yPage);
void ssd1306_DrawText6x8(char * str, uint8_t xPixel, uint8_t yPage);
void ssd1306_DrawText8x8(char * str, uint8_t xPixel, uint8_t yPage);
void ssd1306_DrawChar12x16(char ch, uint8_t xPixel, uint8_t yPage);
void ssd1306_DrawText12x16(char * str, uint8_t xPixel, uint8_t yPage);

#endif 