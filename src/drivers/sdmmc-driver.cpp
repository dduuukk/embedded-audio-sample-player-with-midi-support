#include "sdmmc-driver.h"
#include <stm32h7xx_hal.h>

SDMMC::SDMMC() {
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 4;
}

void HAL_SD_MspInit(SD_HandleTypeDef *hsd) { __HAL_RCC_SDMMC1_CLK_ENABLE(); }
