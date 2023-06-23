#include "main.h"

/*

    KEYBOARD:
RESTORE 42
PORT A  9...16
PORT B  1...8

    LCD:
MISO    37
MOSI    35
SCLK    36
CS      45
A0      39
RST     40
CS      41

*/

void c64_run(void *parameters)
{
    for (;;)
    {
        for (uint32_t i = 0; i < 10; i++)
        {
            exec_ins();
            if (!gpio_get_level(42))
                nmi();
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    ESP_ERROR_CHECK(esp_task_wdt_init(30, false));

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 1024 * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = 1ULL << 42;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    display_init(20);

    esp_timer_create_args_t irq_timer_config = {
        .name = "irq_timer",
        .callback = &irq,
    };
    esp_timer_handle_t irq_timer;
    ESP_ERROR_CHECK(esp_timer_create(&irq_timer_config, &irq_timer));

    reset();
    xTaskCreate(c64_run, "c64_run", 5000, NULL, 2, NULL);
    ESP_ERROR_CHECK(esp_timer_start_periodic(irq_timer, 16666));
}

/*
while (1)
{
    if (pc == 0xe112)
    {
        char buf[2];
        while (!uart_read_bytes(UART_NUM_0, buf, 1, 20 / portTICK_PERIOD_MS))
            ;
        uart_tx_chars(UART_NUM_0, buf, 1);
        a = buf[0];
        if (a == '\n')
            a = '\r';
        pc = 0xe117;
    }
    if (pc == 0xe716)
    {
        if (a == '\r')
            putchar('\n');
        else
            putchar(a);
    }
    exec_ins();
}
*/
