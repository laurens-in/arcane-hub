#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t cycle_config_task_handle;

void can_read_task(void *params);

void cycle_config_task(void *params);

void cdc_task(void *params);

void usbd_task(void *params);

void idle_task(void *params);

void button_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#endif /* TASKS_H */