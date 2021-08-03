

set(srcs "main.c")

idf_component_register(SRCS ${srcs} INCLUDE_DIRS "." REQUIRES lcd_ssd1306)
