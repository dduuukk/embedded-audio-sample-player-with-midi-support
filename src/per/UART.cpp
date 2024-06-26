#include "UART.h"
#include "stm32h7xx_hal_gpio_ex.h"
#include "stm32h7xx_hal_uart.h"

// void Error_Handler(void)
// {
//   __disable_irq();
//   while (1)
//   {
//   }
// }

UART_HandleTypeDef *my_huart;

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (huart->Instance == USART1) {
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB6      ------> USART1_TX
    PB7      ------> USART1_RX
    */
    // GPIO_InitStruct.Pin = GPIO_PIN_6;
    // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // GPIO_InitStruct.Pin = GPIO_PIN_7;
    // GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    // GPIO_InitStruct.Pull = GPIO_NOPULL;
    // HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
}

void initUART1(UART_HandleTypeDef *huart1, uint8_t *rx_buff) {

  huart1->Instance = USART1;
  huart1->Init.BaudRate = 31250;
  //huart1->Init.BaudRate = 4800;
  huart1->Init.WordLength = UART_WORDLENGTH_8B;
  huart1->Init.StopBits = UART_STOPBITS_1;
  huart1->Init.Parity = UART_PARITY_NONE;
  huart1->Init.Mode = UART_MODE_TX_RX;
  huart1->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1->Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(huart1) != HAL_OK) {
    __asm__ __volatile__("bkpt #0");
  }

  my_huart = huart1;
}

extern "C" {

void USART1_IRQHandler(void) { HAL_UART_IRQHandler(my_huart); }
}