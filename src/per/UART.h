#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_uart.h"

#include "MIDI_handler.h"
#include "MIDI_event.h"

midiHandler<16, 16> midi_handler;

UART_HandleTypeDef huart1;

uint8_t rx_buff[8];

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_ENABLE();
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB6      ------> USART1_TX
    PB7      ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
}

static void initUART1(void);
