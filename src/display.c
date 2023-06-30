#include "display.h"

void display_init()
{
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = 0;
    io_conf.pin_bit_mask |= (1ULL << PIN_LCD_RD) | (1ULL << PIN_LCD_WR) | (1ULL << PIN_LCD_RS) | (1ULL << PIN_LCD_CS) | (1ULL << PIN_LCD_RST);
    io_conf.pin_bit_mask |= (1ULL << PIN_LCD_D0) | (1ULL << PIN_LCD_D1) | (1ULL << PIN_LCD_D2) | (1ULL << PIN_LCD_D3) | (1ULL << PIN_LCD_D4) | (1ULL << PIN_LCD_D5) | (1ULL << PIN_LCD_D6) | (1ULL << PIN_LCD_D7);
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_set_level(PIN_LCD_CS, 1);
    gpio_set_level(PIN_LCD_WR, 1);
    gpio_set_level(PIN_LCD_RD, 1);
    gpio_set_level(PIN_LCD_RS, 1);
    gpio_set_level(PIN_LCD_RST, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_LCD_RST, 1);
    gpio_set_level(PIN_LCD_CS, 0);

    display_write_c(0x00);
    display_write_c(0x00);
    display_write_c(0x00);

    uint8_t table[] = {
        0xF1, 6, 0x36, 0x04, 0x00, 0x3C, 0x0F, 0x8F,
        0xF2, 9, 0x18, 0xA3, 0x12, 0x02, 0xB2, 0x12, 0xFF, 0x10, 0x00,
        0xF8, 2, 0x21, 0x04,
        0xF9, 2, 0x00, 0x08,
        0x36, 1, 0x08,
        0xB4, 1, 0x00,
        0xC1, 1, 0x41,
        0xC5, 4, 0x00, 0x91, 0x80, 0x00,
        0xE0, 15, 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98, 0x37, 0x0A, 0x13, 0x04, 0x11, 0x0D, 0x00,
        0xE1, 15, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75, 0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00,
        0x3A, 1, 0x55,
        0x11, 0,
        0x36, 1, 0x28,
        0x7F, 120,
        0x29, 0
        /*
        0x01, 0,            //Soft Reset
        TFTLCD_DELAY8, 150,  // .kbv will power up with ONLY reset, sleep out, display on
        0x28, 0,            //Display Off
        0x3A, 1, 0x55,      //Pixel read=565, write=565.
        0xC0, 2, 0x0d, 0x0d,        //Power Control 1 [0E 0E]
        0xC1, 2, 0x43, 0x00,        //Power Control 2 [43 00]
        0xC2, 1, 0x00,      //Power Control 3 [33]
        0xC5, 4, 0x00, 0x48, 0x00, 0x48,    //VCOM  Control 1 [00 40 00 40]
        0xB4, 1, 0x00,      //Inversion Control [00]
        0xB6, 3, 0x02, 0x02, 0x3B,  // Display Function Control [02 02 3B]
        0xE0, 15,0x0F, 0x24, 0x1C, 0x0A, 0x0F, 0x08, 0x43, 0x88, 0x32, 0x0F, 0x10, 0x06, 0x0F, 0x07, 0x00,
        0xE1, 15,0x0F, 0x38, 0x30, 0x09, 0x0F, 0x0F, 0x4E, 0x77, 0x3C, 0x07, 0x10, 0x05, 0x23, 0x1B, 0x00,
        0x11, 0,            //Sleep Out
        TFTLCD_DELAY8, 150,
        0x29, 0         //Display On
        */
    };
    for (uint8_t i = 0; i < sizeof(table); i += 2)
    {
        if (table[i] == 0x7f)
            vTaskDelay(table[i + 1]);
        else
            display_write_cd_push(table[i], &table[i + 2], table[i + 1]);
        i += table[i + 1];
    }

    uint8_t val;
    switch (1)
    {
    case 0:
        val = 0x48; // 0 degree
        break;
    case 1:
        val = 0x38; // 90 degree
        break;
    case 2:
        val = 0x88; // 180 degree
        break;
    case 3:
        val = 0xe8; // 270 degree
        break;
    }
    // display_write_cd(0x36, val);
}

bool cmd = 0;

void display_write(uint8_t data)
{
    printf("%c %2x\n", cmd ? 'C' : 'D', data);
    gpio_set_level(PIN_LCD_D0, (data >> 0) & 1);
    gpio_set_level(PIN_LCD_D1, (data >> 1) & 1);
    gpio_set_level(PIN_LCD_D2, (data >> 2) & 1);
    gpio_set_level(PIN_LCD_D3, (data >> 3) & 1);
    gpio_set_level(PIN_LCD_D4, (data >> 4) & 1);
    gpio_set_level(PIN_LCD_D5, (data >> 5) & 1);
    gpio_set_level(PIN_LCD_D6, (data >> 6) & 1);
    gpio_set_level(PIN_LCD_D7, (data >> 7) & 1);
    gpio_set_level(PIN_LCD_WR, 0);
    gpio_set_level(PIN_LCD_WR, 1);
}

void display_write16(uint16_t data)
{
    display_write(data >> 8);
    display_write(data & 0xff);
}

void display_write_c(uint8_t command)
{
    gpio_set_level(PIN_LCD_RS, 0);
    cmd = 1;
    display_write(command);
    cmd = 0;
    gpio_set_level(PIN_LCD_RS, 1);
}

void display_write_c16(uint16_t command)
{
    gpio_set_level(PIN_LCD_RS, 0);
    cmd = 1;
    display_write16(command);
    cmd = 0;
    gpio_set_level(PIN_LCD_RS, 1);
}

void display_write_cd(uint8_t command, uint8_t data)
{
    display_write_c(command);
    display_write(data);
}

void display_write_cd16(uint16_t command, uint16_t data)
{
    display_write_c16(command);
    display_write16(data);
}

void display_write_cd_push(uint16_t command, uint8_t *data, uint8_t len)
{
    display_write_c16(command);
    for (uint8_t i = 0; i < len; i++)
        display_write(data[i]);
}

void display_set_pixel(uint16_t x, uint16_t y, bool state)
{
    uint16_t b = 0b0010100010100101;
    uint16_t d = 0b0000100000100001;

    display_write_cd16(0x2A, x);
    display_write16(x);
    display_write_cd16(0x2B, y);
    display_write16(y);
    display_write_cd16(0x2C, state ? b : d);
}
