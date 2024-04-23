#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_uart.h"

#include "MIDI_event.h"
#include "MIDI_handler.h"
#include <cstdint>

#ifndef UART_H
#define UART_H

void HAL_UART_MspInit(UART_HandleTypeDef *huart);

void initUART1(UART_HandleTypeDef *huart1, uint8_t *rx_buff);

#endif
