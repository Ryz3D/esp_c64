#include "main.h"

int64_t last_irq = 0;

void c64_run(void *parameters)
{
    pc = 0xc000;
    int64_t t_start = esp_timer_get_time();
    for (;;)
    {
        for (uint32_t i = 0; i < 10000; i++)
        {
            if (esp_timer_get_time() - last_irq > 16666)
            {
                irq();
                last_irq = esp_timer_get_time();
            }
            if (!gpio_get_level(42))
                nmi();

            if (pc == 0xfce2)
            {
                int64_t t_end = esp_timer_get_time();
                int32_t t_duration = t_end - t_start;
                printf("%i\n", t_duration);
            }
            if (pc == 0xe5cd)
            {
                int c = getchar();
                if (c > 0)
                {
                    if (c == '\n')
                        c = '\r';
                    // siehe 0xEB35
                    uint8_t kb_index = (uint8_t)mem_read(0xc6);
                    mem_write(0x0277 + kb_index, c);
                    mem_write(0xc6, kb_index + 1);
                }
            }
            if (pc == 0xe716)
            {
                if (a == '\r')
                    putchar('\n');
                else if (a == 0x1d)
                    putchar(' ');
                else if ((uint8_t)a != 0x93)
                    putchar(a);
            }
            exec_ins();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
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

    // display_init(20);

    reset();
    xTaskCreate(c64_run, "c64_run", 5000, NULL, 2, NULL);
}
