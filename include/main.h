#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_task_wdt.h"
#include "sdkconfig.h"

#include "display.h"
#include "cpu.h"

void c64_run(void *parameters);
void app_main();
