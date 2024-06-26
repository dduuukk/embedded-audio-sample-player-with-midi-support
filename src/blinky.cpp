#include "MIDI_event.h"
#include "fatfs.h"
#include "ff.h"
#include "sai-driver.h"
#include <cstdint>
#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_gpio.h>
#include <stm32h7xx_hal_uart.h>
#include <string>

#include "MIDI_handler.h"
#include "codec_wm8731.h"
#include "ff.h"
#include "stm32h7xx_hal_conf.h"
#include "wav-parser.h"

#include "UART.h"

midiHandler<16, 16> midi_handler;
uint8_t rx_buff[8];

UART_HandleTypeDef huart1 = {};

#define LED_PORT GPIOC
#define LED_PIN GPIO_PIN_0
#define LED_PORT_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE

// The buffer to read samples into
uint8_t FATFS_BUFFER_MEM_SECTION buff[163840];

void clocks_initialise(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable
   */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
   ** and set PLLN value, and flash-latency.
   **
   ** See page 159 of Reference manual for VOS/Freq relationship
   ** and table for flash latency.
   */

  uint32_t plln_val, flash_latency;

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
  plln_val = 240;
  flash_latency = FLASH_LATENCY_4;

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
  }
  /** Macro to configure the PLL clock source
   */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType =
      RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  //
  RCC_OscInitStruct.PLL.PLLN = plln_val;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5; // was 4 in cube
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    __asm__ __volatile__("bkpt #0");
  }
  /** Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flash_latency) != HAL_OK) {
    __asm__ __volatile__("bkpt #0");
  }
  PeriphClkInitStruct.PeriphClockSelection =
      RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART6 | RCC_PERIPHCLK_USART234578 |
      RCC_PERIPHCLK_LPUART1 | RCC_PERIPHCLK_RNG | RCC_PERIPHCLK_SPI1 |
      RCC_PERIPHCLK_SAI2 | RCC_PERIPHCLK_SAI1 | RCC_PERIPHCLK_SDMMC |
      RCC_PERIPHCLK_I2C2 | RCC_PERIPHCLK_ADC | RCC_PERIPHCLK_I2C1 |
      RCC_PERIPHCLK_USB | RCC_PERIPHCLK_QSPI | RCC_PERIPHCLK_FMC;
  // PLL 2
  //  PeriphClkInitStruct.PLL2.PLL2N = 115; // Max Freq @ 3v3
  // PeriphClkInitStruct.PLL2.PLL2N      = 84; // Max Freq @ 1V9
  /** Old Timing  */
  /*
  PeriphClkInitStruct.PLL2.PLL2N      = 100; // Max supported freq of FMC;
  PeriphClkInitStruct.PLL2.PLL2M      = 4;
  PeriphClkInitStruct.PLL2.PLL2P      = 8;  // 57.5
  PeriphClkInitStruct.PLL2.PLL2Q      = 10; // 46
  PeriphClkInitStruct.PLL2.PLL2R      = 2;  // 115Mhz
  PeriphClkInitStruct.PLL2.PLL2FRACN  = 0;
  */

  // New Timing
  PeriphClkInitStruct.PLL2.PLL2N = 12; // Max supported freq of FMC;
  PeriphClkInitStruct.PLL2.PLL2M = 1;
  PeriphClkInitStruct.PLL2.PLL2P = 8; // 25MHz
  PeriphClkInitStruct.PLL2.PLL2Q = 2; // 100MHz
  PeriphClkInitStruct.PLL2.PLL2R = 1; // 200MHz
  PeriphClkInitStruct.PLL2.PLL2FRACN = 4096;

  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  // PLL 3
  PeriphClkInitStruct.PLL3.PLL3M = 6;
  PeriphClkInitStruct.PLL3.PLL3N = 295;
  PeriphClkInitStruct.PLL3.PLL3P = 16; // 49.xMhz
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 32; // 24.xMhz
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;
  PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
  // PeriphClkInitStruct.SdmmcClockSelection  = RCC_SDMMCCLKSOURCE_PLL;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;
  PeriphClkInitStruct.Sai23ClockSelection = RCC_SAI23CLKSOURCE_PLL3;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL2;
  PeriphClkInitStruct.Usart234578ClockSelection =
      RCC_USART234578CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PLL3;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    __asm__ __volatile__("bkpt #0");
  }

  /** Enable USB Voltage detector */
  HAL_PWREx_EnableUSBVoltageDetector();

  HAL_NVIC_DisableIRQ(SysTick_IRQn);

  HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

void initGPIO() {
  GPIO_InitTypeDef GPIO_Config;

  GPIO_Config.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Config.Pull = GPIO_NOPULL;
  GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_Config.Pin = LED_PIN;

  LED_PORT_CLK_ENABLE();
  HAL_GPIO_Init(LED_PORT, &GPIO_Config);

  GPIO_Config.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Config.Pull = GPIO_NOPULL;
  GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_Config.Pin = GPIO_PIN_3;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  HAL_GPIO_Init(GPIOA, &GPIO_Config);
}

std::string boombaFiles[25] = {
    "boomba-1.wav",  "boomba-2.wav",  "boomba-3.wav",  "boomba-4.wav",
    "boomba-5.wav",  "boomba-6.wav",  "boomba-7.wav",  "boomba-8.wav",
    "boomba-9.wav",  "boomba-10.wav", "boomba-11.wav", "boomba-12.wav",
    "boomba-13.wav", "boomba-14.wav", "boomba-15.wav", "boomba-16.wav",
    "boomba-17.wav", "boomba-18.wav", "boomba-19.wav", "boomba-20.wav",
    "boomba-21.wav", "boomba-22.wav", "boomba-23.wav", "boomba-24.wav",
    "boomba-25.wav"};

std::string drumFiles[10] = {"hell-naw.wav",         "big-boomba.wav",
                             "metal-pipe-short.wav", "bruh.wav",
                             "honk-bonk.wav",        "omg.wav",
                             "BENZ_-_Snare_16.wav",  "BENZ_-_Kick_04.wav",
                             "BENZ_-_Hi_Hat_05.wav", "808-short.wav"};

int main(void) {
  HAL_Init();
  initGPIO();
  clocks_initialise();
  HAL_SYSTICK_Config(SystemCoreClock / 1000);
  HAL_InitTick(1UL << (__NVIC_PRIO_BITS - 1));

  initUART1(&huart1, rx_buff);
  if (HAL_UART_Receive_IT(&huart1, rx_buff, 1) != HAL_OK) {
    __asm__ __volatile__("bkpt #0");
  }

  FatFsIntf fatFsIntf = FatFsIntf();
  struct wave_header wavHeader;
  FIL fp;
  UINT bRead;

  WM8731 codec = WM8731();
  codec.init();
  codec.configureSampleRate(ADC_44k1_DAC_44k1);
  codec.configureInputDataLength(INPUT_16BITS);
  codec.configureBypass(BYPASS_DISABLE);

  SAIDriver newSaiDriver = SAIDriver(true, SAIDriver::BitDepth::BIT_DEPTH_16,
                                     SAIDriver::SampleRate::SAMPLE_RATE_44K);

  // newSaiDriver.txTransmit((buff), 65535, 2000);

  while (1) {
    if (midi_handler.midiRecieveCheckEmpty() == false) {
      midi_handler.parse(midi_handler.dequeueByte());
    }

    if (midi_handler.midiEventCheckEmpty() == false) {
      MidiEvent event = midi_handler.dequeueEvent();

      if (event.channel == 0 && event.messageType == NoteOn &&
          event.data[0] == 48 && event.data[1] > 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
      } else if (event.channel == 0 && event.messageType == NoteOff &&
                 event.data[0] == 48 && event.data[1] == 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
      }
      if (event.channel == 0 && event.messageType == NoteOn) {
        if (f_open(&fp, boombaFiles[event.data[0] - 48].c_str(),
                   FA_READ | FA_OPEN_EXISTING) != FR_OK) {
          __asm__ __volatile__("bkpt #0");
        }
        if (f_read(&fp, buff, 80896, &bRead) != FR_OK) {
          __asm__ __volatile__("bkpt #0");
        }

        // read file header
        read_wave(buff, &wavHeader);

        // parse file header, verify that is wave
        if (validate_wave(&wavHeader) != 0) {
          return -1;
        }
        play_wave_samples(buff, &wavHeader, 32596, 128, newSaiDriver);

        f_close(&fp);
      } else if (event.channel == 9 && event.messageType == NoteOn) {
        uint32_t toRead = 0;
        switch (event.data[0] - 36) {
        case 0:
          toRead = 151552;
          break;
        case 1:
          toRead = 81920;
          break;
        case 2:
          toRead = 163840;
          break;
        case 3:
          toRead = 122880;
          break;
        case 4:
          toRead = 143360;
          break;
        case 5:
          toRead = 143360;
          break;
        case 6:
          toRead = 106496;
          break;
        case 7:
          toRead = 118784;
          break;
        case 8:
          toRead = 69632;
          break;
        case 9:
          toRead = 163840;
          break;
        default:
          toRead = 0;
          break;
        }
        if (toRead == 0) {
          continue;
        }
        if (f_open(&fp, drumFiles[event.data[0] - 36].c_str(),
                   FA_READ | FA_OPEN_EXISTING) != FR_OK) {
          __asm__ __volatile__("bkpt #0");
        }
        if (f_read(&fp, buff, toRead, &bRead) != FR_OK) {
          __asm__ __volatile__("bkpt #0");
        }

        // read file header
        read_wave(buff, &wavHeader);

        // parse file header, verify that is wave
        if (validate_wave(&wavHeader) != 0) {
          return -1;
        }
        play_wave_samples(buff, &wavHeader, toRead / 4, 128, newSaiDriver);

        f_close(&fp);
      }
    }
  }

  return 0;
}

// This prevent name mangling for functions used in C/assembly files.
extern "C" {
void SysTick_Handler(void) {
  // __asm__ __volatile__("bkpt #0");
  HAL_IncTick();

  // 1 Hz blinking
  if ((HAL_GetTick() % 1000) == 0) {
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
  }
  HAL_SYSTICK_IRQHandler();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  // HAL_UART_Receive_IT(&huart1, rx_buff, 1);
  // midi_handler.enqueueByte(rx_buff[0]);

  if (huart->Instance == USART1) {
    HAL_UART_Receive_IT(huart, rx_buff, 1);
    midi_handler.enqueueByte(rx_buff[0]);
    //__asm__ __volatile__("bkpt #0");
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  volatile uint32_t wompwomp = huart->ErrorCode;
  __asm__ __volatile__("bkpt #0");
}
}