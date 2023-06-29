#include "display.h"

uint8_t display_buffer[1024];

void display_init(uint8_t contrast)
{
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // SDA, SCL, A0, RST, CS
    io_conf.pin_bit_mask = (1ULL << 35) | (1ULL << 36) | (1ULL << 39) | (1ULL << 40) | (1ULL << 41);
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_set_level(40, 0); // RST
    gpio_set_level(41, 0); // CS
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(40, 1); // RST

    display_write_c(0xa0);
    display_write_c(0xc8);
    display_write_c(0xa6);
    display_write_c(0xa2);
    display_write_c(0x2f);
    display_write_c(0xf8);
    display_write_c(0x00);
    display_write_c(0x27);
    display_write_c(0x81);
    display_write_c(contrast);
    display_write_c(0xac);
    display_write_c(0x00);
    display_write_c(0xa4);
    display_write_c(0xaf);
}

void shift_out_impl(uint8_t val)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        gpio_set_level(35, (val & 128) != 0);
        val <<= 1;

        gpio_set_level(36, 1);
        gpio_set_level(36, 0);
    }
}

void display_write_c(uint8_t command)
{
    gpio_set_level(39, 0); // A0
    shift_out_impl(command);
    gpio_set_level(39, 1); // A0
}

void display_write_d(uint8_t data)
{
    shift_out_impl(data);
}

void display_set_page(uint8_t page)
{
    display_write_c(page | 0xb0);
}

void display_set_col(uint8_t col)
{
    uint8_t high_nibble = col >> 4;
    display_write_c(0x10 | high_nibble);
    uint8_t low_nibble = col & 0x0f;
    display_write_c(low_nibble);
}

void display_set_pixel(uint16_t x, uint16_t y, bool state)
{
    uint16_t pixel_index = x + (y / 8) * 128;
    if (state)
        display_buffer[pixel_index] |= 1 << (y % 8);
    else
        display_buffer[pixel_index] &= ~(1 << (y % 8));
    display_set_page(pixel_index / 128);
    display_set_col(pixel_index % 128);
    display_write_d(display_buffer[pixel_index]);
}

void display_show()
{
    uint16_t i = 0;
    for (uint8_t page = 0; page < 8; page++)
    {
        display_set_page(page);
        display_set_col(0);
        for (uint8_t column = 0; column < 128; column++)
            display_write_d(display_buffer[i++]);
    }
}
