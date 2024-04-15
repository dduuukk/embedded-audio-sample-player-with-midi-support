#include "sdmmc-driver.h"
#include <stm32h7xx_hal.h>

SD_HandleTypeDef hsd1 = {};

SDMMC::SDMMC() {
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 250;
}

// Based on Daisy Seed Example Code
void HAL_SD_MspInit(SD_HandleTypeDef *sdHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (sdHandle->Instance == SDMMC1) {
    /* SDMMC1 clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration
        PC12     ------> SDMMC1_CK
        PC11     ------> SDMMC1_D3
        PC10     ------> SDMMC1_D2
        PD2     ------> SDMMC1_CMD
        PC9     ------> SDMMC1_D1
        PC8     ------> SDMMC1_D0
    */

    uint32_t gpioSpeed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_8;
    if (sdHandle->Init.BusWide == SDMMC_BUS_WIDE_4B)
      GPIO_InitStruct.Pin |= GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gpioSpeed;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = gpioSpeed;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDMMC1 interrupt Init */
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
    /* USER CODE BEGIN SDMMC1_MspInit 1 */

    /* USER CODE END SDMMC1_MspInit 1 */
  }
}

extern "C" {
void SDMMC1_IRQHandler() { HAL_SD_IRQHandler(&hsd1); }
}