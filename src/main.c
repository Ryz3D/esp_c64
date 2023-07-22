#include "main.h"

int64_t t_f_cpu_start = 0;
int64_t t_last_irq = 0;

void handle_uart_io()
{
    if (ram[0xff0] != 0)
    {
        if (pc == 0xe5cd)
        {
            // siehe 0xEB35
            uint8_t kb_index = (uint8_t)mem_read(0xc6);
            if (kb_index < (uint8_t)mem_read(0x0289))
            {
                int c = getchar();
                if (c > 0)
                {
                    if (c == '\n')
                        c = '\r';
                    mem_write(0x0277 + kb_index, c);
                    mem_write(0xc6, kb_index + 1);
                }
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
    }
}

void loop_f_cpu_check()
{
    if (pc == 0xc000)
    {
        t_f_cpu_start = esp_timer_get_time();
    }
    if (pc == 0xfce2 && t_f_cpu_start != 0)
    {
        int32_t t_duration = esp_timer_get_time() - t_f_cpu_start;
        float f_cpu = 328714 / (t_duration / 1000000.0);
        printf("F_CPU = %i Hz\n", (int)f_cpu);
        t_f_cpu_start = 0;
    }
}

void c64_run(void *parameters)
{
    reset();
    ram[0xff0] = ram[0xff1] = ram[0xff2] = 1;
    while (1)
    {
        for (uint32_t i = 0; i < 10000; i++)
        {
            if (esp_timer_get_time() - t_last_irq > 16666)
            {
                irq();
                t_last_irq = esp_timer_get_time();
            }
            if (!gpio_get_level(42))
                nmi();
            if (pc == 0xe544)
                display_clear();

            handle_uart_io();
            loop_f_cpu_check();
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
    io_conf.pin_bit_mask = (1ULL << 42) | (1ULL << 17);
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    display_init();

    xTaskCreate(c64_run, "c64_run", 5000, NULL, 2, NULL);
}
