#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include <driver/spi_master.h>

extern uint8_t display_buffer[1024];

void display_init(uint8_t contrast);
void display_write_c(uint8_t command);
void display_write_d(uint8_t data);
void display_set_page(uint8_t page);
void display_set_col(uint8_t col);
void display_set_pixel(uint16_t x, uint16_t y, bool state);
void display_show();
