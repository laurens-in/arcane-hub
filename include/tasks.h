#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h"
#include "task.h"

/** Handle for the cycle configuration task */
extern TaskHandle_t cycle_config_task_handle;

/**
 * @brief Task to read CAN messages.
 *
 * This task continuously checks for incoming CAN messages. When a message is received,
 * it processes the message and performs actions based on the function code contained in
 * the message.
 *
 * @param params Pointer to task parameters (not used).
 */
void can_read_task(void *params);

/**
 * @brief Task to cycle node configuration options.
 *
 * This task waits for notifications from the user button IRQ and then cycles through configurations.
 * Specifically it sends configuration messages to the node with id 1, updating its MIDI channel from 0-8.
 *
 * @param params Pointer to task parameters (not used).
 */
void cycle_config_task(void *params);

/**
 * @brief Task to handle CDC communications.
 *
 * This task checks if the device is connected via CDC and if there are any
 * available configuration requests. It reads the data, processes it, and sends appropriate
 * messages.
 *
 * @param params Pointer to task parameters (not used).
 */
void cdc_task(void *params);

/**
 * @brief Task to handle USB device operations.
 *
 * This task initializes the USB device stack and processes USB device tasks.
 * It should be called after the RTOS scheduler has started.
 *
 * @param params Pointer to task parameters (not used).
 */
void usbd_task(void *params);

/**
 * @brief Idle task.
 *
 * This task runs when no other tasks are ready to run.
 *
 * @param params Pointer to task parameters (not used).
 */
void idle_task(void *params);

/**
 * @brief IRQ handler for the user button.
 *
 * This function is called when an interrupt is triggered by the user button.
 * It notifies the cycle configuration task to perform an action.
 *
 * @param pin The pin number where the interrupt occurred.
 * @param action The action that triggered the interrupt.
 */
void button_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#endif /* TASKS_H */