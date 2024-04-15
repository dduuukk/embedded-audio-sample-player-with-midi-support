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

/**
 * @brief  Wrap up reading in non-blocking mode.
 * @param  hsd: pointer to a SD_HandleTypeDef structure that contains
 *              the configuration information.
 * @retval None
 */
static void SD_Read_IT(SD_HandleTypeDef *hsd) {
  uint32_t count, data;
  uint8_t *tmp;

  tmp = hsd->pRxBuffPtr;

  /* Read data from SDMMC Rx FIFO */
  for (count = 0U; count < 8U; count++) {
    data = SDMMC_ReadFIFO(hsd->Instance);
    *tmp = (uint8_t)(data & 0xFFU);
    tmp++;
    *tmp = (uint8_t)((data >> 8U) & 0xFFU);
    tmp++;
    *tmp = (uint8_t)((data >> 16U) & 0xFFU);
    tmp++;
    *tmp = (uint8_t)((data >> 24U) & 0xFFU);
    tmp++;
  }

  hsd->pRxBuffPtr = tmp;
}

/**
 * @brief  Wrap up writing in non-blocking mode.
 * @param  hsd: pointer to a SD_HandleTypeDef structure that contains
 *              the configuration information.
 * @retval None
 */
static void SD_Write_IT(SD_HandleTypeDef *hsd) {
  uint32_t count;
  uint32_t data;
  uint8_t *tmp;

  tmp = hsd->pTxBuffPtr;

  if (hsd->TxXferSize >= 32U) {
    /* Write data to SDMMC Tx FIFO */
    for (count = 0U; count < 8U; count++) {
      data = (uint32_t)(*tmp);
      tmp++;
      data |= ((uint32_t)(*tmp) << 8U);
      tmp++;
      data |= ((uint32_t)(*tmp) << 16U);
      tmp++;
      data |= ((uint32_t)(*tmp) << 24U);
      tmp++;
      (void)SDMMC_WriteFIFO(hsd->Instance, &data);
    }

    hsd->pTxBuffPtr = tmp;
    hsd->TxXferSize -= 32U;
  }
}
/**
 * @brief  This function handles SD card interrupt request.
 * @param  hsd: Pointer to SD handle
 * @retval None
 */
void modded_HAL_SD_IRQHandler(SD_HandleTypeDef *hsd) {
  uint32_t errorstate;
  uint32_t context = hsd->Context;

  __asm__ __volatile__("bkpt #0");

  /* Check for SDMMC interrupt flags */
  if (__HAL_SD_GET_FLAG(hsd, SDMMC_IT_DATAEND) != RESET) {
    __HAL_SD_CLEAR_FLAG(hsd, SDMMC_FLAG_DATAEND);

    __HAL_SD_DISABLE_IT(hsd, SDMMC_IT_DATAEND | SDMMC_IT_DCRCFAIL |
                                 SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR |
                                 SDMMC_IT_RXOVERR | SDMMC_IT_TXFIFOHE |
                                 SDMMC_IT_RXFIFOHF);

    __HAL_SD_DISABLE_IT(hsd, SDMMC_IT_IDMABTC);
    __SDMMC_CMDTRANS_DISABLE(hsd->Instance);

    if ((context & SD_CONTEXT_IT) != 0U) {
      if (((context & SD_CONTEXT_READ_MULTIPLE_BLOCK) != 0U) ||
          ((context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U)) {
        errorstate = SDMMC_CmdStopTransfer(hsd->Instance);
        if (errorstate != HAL_SD_ERROR_NONE) {
          hsd->ErrorCode |= errorstate;
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
          hsd->ErrorCallback(hsd);
#else
          HAL_SD_ErrorCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
        }
      }

      /* Clear all the static flags */
      __HAL_SD_CLEAR_FLAG(hsd, SDMMC_STATIC_DATA_FLAGS);

      hsd->State = HAL_SD_STATE_READY;
      if (((context & SD_CONTEXT_READ_SINGLE_BLOCK) != 0U) ||
          ((context & SD_CONTEXT_READ_MULTIPLE_BLOCK) != 0U)) {
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->RxCpltCallback(hsd);
#else
        HAL_SD_RxCpltCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
      } else {
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->TxCpltCallback(hsd);
#else
        HAL_SD_TxCpltCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
      }
    } else if ((context & SD_CONTEXT_DMA) != 0U) {
      hsd->Instance->DLEN = 0;
      hsd->Instance->DCTRL = 0;
      hsd->Instance->IDMACTRL = SDMMC_DISABLE_IDMA;

      /* Stop Transfer for Write Multi blocks or Read Multi blocks */
      if (((context & SD_CONTEXT_READ_MULTIPLE_BLOCK) != 0U) ||
          ((context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U)) {
        errorstate = SDMMC_CmdStopTransfer(hsd->Instance);
        if (errorstate != HAL_SD_ERROR_NONE) {
          hsd->ErrorCode |= errorstate;
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
          hsd->ErrorCallback(hsd);
#else
          HAL_SD_ErrorCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
        }
      }

      hsd->State = HAL_SD_STATE_READY;
      if (((context & SD_CONTEXT_WRITE_SINGLE_BLOCK) != 0U) ||
          ((context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U)) {
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->TxCpltCallback(hsd);
#else
        HAL_SD_TxCpltCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
      }
      if (((context & SD_CONTEXT_READ_SINGLE_BLOCK) != 0U) ||
          ((context & SD_CONTEXT_READ_MULTIPLE_BLOCK) != 0U)) {
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->RxCpltCallback(hsd);
#else
        HAL_SD_RxCpltCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
      }
    } else {
      /* Nothing to do */
    }
  }

  else if ((__HAL_SD_GET_FLAG(hsd, SDMMC_IT_TXFIFOHE) != RESET) &&
           ((context & SD_CONTEXT_IT) != 0U)) {
    SD_Write_IT(hsd);
  }

  else if ((__HAL_SD_GET_FLAG(hsd, SDMMC_IT_RXFIFOHF) != RESET) &&
           ((context & SD_CONTEXT_IT) != 0U)) {
    SD_Read_IT(hsd);
  }

  else if (__HAL_SD_GET_FLAG(hsd, SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT |
                                      SDMMC_IT_RXOVERR | SDMMC_IT_TXUNDERR) !=
           RESET) {
    if (__HAL_SD_GET_FLAG(hsd, SDMMC_FLAG_IDMATE) != RESET) {
      __asm__ __volatile__("bkpt #0");
    }
    /* Set Error code */
    if (__HAL_SD_GET_FLAG(hsd, SDMMC_IT_DCRCFAIL) != RESET) {
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_CRC_FAIL;
    }
    if (__HAL_SD_GET_FLAG(hsd, SDMMC_IT_DTIMEOUT) != RESET) {
      hsd->ErrorCode |= HAL_SD_ERROR_DATA_TIMEOUT;
    }
    if (__HAL_SD_GET_FLAG(hsd, SDMMC_IT_RXOVERR) != RESET) {
      hsd->ErrorCode |= HAL_SD_ERROR_RX_OVERRUN;
    }
    if (__HAL_SD_GET_FLAG(hsd, SDMMC_IT_TXUNDERR) != RESET) {
      hsd->ErrorCode |= HAL_SD_ERROR_TX_UNDERRUN;
    }

    /* Clear All flags */
    __HAL_SD_CLEAR_FLAG(hsd, SDMMC_STATIC_DATA_FLAGS);

    /* Disable all interrupts */
    __HAL_SD_DISABLE_IT(hsd, SDMMC_IT_DATAEND | SDMMC_IT_DCRCFAIL |
                                 SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR |
                                 SDMMC_IT_RXOVERR);

    __SDMMC_CMDTRANS_DISABLE(hsd->Instance);
    hsd->Instance->DCTRL |= SDMMC_DCTRL_FIFORST;
    hsd->Instance->CMD |= SDMMC_CMD_CMDSTOP;
    hsd->ErrorCode |= SDMMC_CmdStopTransfer(hsd->Instance);
    hsd->Instance->CMD &= ~(SDMMC_CMD_CMDSTOP);
    __HAL_SD_CLEAR_FLAG(hsd, SDMMC_FLAG_DABORT);

    if ((context & SD_CONTEXT_IT) != 0U) {
      /* Set the SD state to ready to be able to start again the process */
      hsd->State = HAL_SD_STATE_READY;
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
      hsd->ErrorCallback(hsd);
#else
      HAL_SD_ErrorCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
    } else if ((context & SD_CONTEXT_DMA) != 0U) {
      if (hsd->ErrorCode != HAL_SD_ERROR_NONE) {
        /* Disable Internal DMA */
        __HAL_SD_DISABLE_IT(hsd, SDMMC_IT_IDMABTC);
        hsd->Instance->IDMACTRL = SDMMC_DISABLE_IDMA;

        /* Set the SD state to ready to be able to start again the process */
        hsd->State = HAL_SD_STATE_READY;
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->ErrorCallback(hsd);
#else
        HAL_SD_ErrorCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
      }
    } else {
      /* Nothing to do */
    }
  }

  else if (__HAL_SD_GET_FLAG(hsd, SDMMC_IT_IDMABTC) != RESET) {
    if (READ_BIT(hsd->Instance->IDMACTRL, SDMMC_IDMA_IDMABACT) == 0U) {
      /* Current buffer is buffer0, Transfer complete for buffer1 */
      if ((hsd->Context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U) {
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->Write_DMADblBuf1CpltCallback(hsd);
#else
        HAL_SDEx_Write_DMADoubleBuffer1CpltCallback(hsd);
#endif       /* USE_HAL_SD_REGISTER_CALLBACKS */
      } else /* SD_CONTEXT_READ_MULTIPLE_BLOCK */
      {
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->Read_DMADblBuf1CpltCallback(hsd);
#else
        HAL_SDEx_Read_DMADoubleBuffer1CpltCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
      }
    } else /* SD_DMA_BUFFER1 */
    {
      /* Current buffer is buffer1, Transfer complete for buffer0 */
      if ((context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U) {
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->Write_DMADblBuf0CpltCallback(hsd);
#else
        HAL_SDEx_Write_DMADoubleBuffer0CpltCallback(hsd);
#endif       /* USE_HAL_SD_REGISTER_CALLBACKS */
      } else /* SD_CONTEXT_READ_MULTIPLE_BLOCK */
      {
#if defined(USE_HAL_SD_REGISTER_CALLBACKS) &&                                  \
    (USE_HAL_SD_REGISTER_CALLBACKS == 1U)
        hsd->Read_DMADblBuf0CpltCallback(hsd);
#else
        HAL_SDEx_Read_DMADoubleBuffer0CpltCallback(hsd);
#endif /* USE_HAL_SD_REGISTER_CALLBACKS */
      }
    }
    __HAL_SD_CLEAR_FLAG(hsd, SDMMC_FLAG_IDMABTC);
  } else {
    /* Nothing to do */
  }
}

extern "C" {
void SDMMC1_IRQHandler() { modded_HAL_SD_IRQHandler(&hsd1); }
}