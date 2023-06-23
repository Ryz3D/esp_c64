#include "display.h"

// display_buffer[x + y * 16] |= 1 << (y % 8);

uint8_t display_buffer[1024];
spi_device_handle_t spi;

void display_init(uint8_t contrast)
{
    // MSB first?
    spi_bus_config_t buscfg = {
        .miso_io_num = 37,
        .mosi_io_num = 35,
        .sclk_io_num = 36,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 16 * 320 * 2 + 8,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = 45,
        .queue_size = 7,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));

    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL << 39) | (1ULL << 40) | (1ULL << 41); // A0, RST, CS
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

void display_write_c(uint8_t command)
{
    gpio_set_level(39, 0); // A0
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 1;
    t.tx_buffer = &command;
    t.user = (void *)0;
    spi_device_transmit(spi, &t);
    gpio_set_level(39, 1); // A0
}

void display_write_d(uint8_t data)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 1;
    t.tx_buffer = &data;
    t.user = (void *)0;
    spi_device_transmit(spi, &t);
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
