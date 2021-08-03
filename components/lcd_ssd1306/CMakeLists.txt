set(srcsa "lcd_fonts.c" "lcd_lib_ssd1306.c")

idf_component_register(SRCS ${srcsa} INCLUDE_DIRS ".")
