#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_uart.h"

#include "MIDI_handler.h"
#include "MIDI_event.h"

midiHandler<16, 16> midi_handler;

UART_HandleTypeDef huart1;

uint8_t rx_buff[8];

void HAL_UART_MspInit(UART_HandleTypeDef* huart);

static void initUART1(void);
