#ifndef SDMMC_DRIVER_H
#define SDMMC_DRIVER_H

#include "stm32h7xx_hal.h"

class SDMMC {
public:
  SDMMC();

private:
  SD_HandleTypeDef hsd1;
};

#endif // SDMMC_DRIVER_H