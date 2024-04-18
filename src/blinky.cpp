#include "fatfs.h"
#include "ff.h"
#include "sai-driver.h"
#include <cstdint>
#include <stm32h7xx_hal.h>

#include "codec_wm8731.h"
#include "ff.h"
#include "wav-parser.h"

#define LED_PORT GPIOC
#define LED_PIN GPIO_PIN_0
#define LED_PORT_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE

// The buffer to read samples into
uint8_t FATFS_BUFFER_MEM_SECTION buff[163840];

// Constants for the sine wave
const int frequency = 800; // Frequency in Hz
const int amplitude = 2000000000;
const int sampleRate = 48000; // Sample rate in Hz (samples per second)
// const float duration = 1.0;     // Duration of the wave in seconds
// const int maxSamples = static_cast<int>(sampleRate * duration) * 2;

// Generate triangle wave
void generateTriangleWave(int frequency, int amplitude, int sampleRate,
                          int32_t *wave) {
  const int numSamples = sampleRate / frequency;
  const long long incrementL =
      (static_cast<long long>(2) * static_cast<long long>(amplitude)) /
      (static_cast<long long>(numSamples) / 2LL);
  const int increment = static_cast<int>(incrementL);

  int sample = 0;
  for (int i = 0; i < numSamples / 2; ++i) {
    wave[i] = sample;
    wave[i + 1] = sample;
    sample += increment;
    i++;
  }

  for (int i = numSamples / 2; i < 3 * numSamples / 2; ++i) {
    wave[i] = sample;
    wave[i + 1] = sample;
    sample -= increment;
    i++;
  }

  for (int i = 3 * numSamples / 2; i < numSamples * 2; ++i) {
    wave[i] = sample;
    wave[i + 1] = sample;
    sample += increment;
    i++;
  }
}

// // Generate the sine wave
// void generateSineWave(float frequency, float amplitude, float sampleRate,
// float duration, int32_t* wave) {
//     const float twoPiF = 2.0 * M_PI * frequency;
//     const int numSamples = static_cast<int>(duration * sampleRate);
//     if(numSamples > maxSamples) {
//       // Error: too many samples
//       return;
//     }
//     for (int i = 0; i < numSamples; ++i) {
//       float t = i / sampleRate;
//       float sample = amplitude * sin(twoPiF * t);
//       // Normalize the sample from [-1,1] to [int32_min, int32_max]
//       wave[i] = static_cast<int32_t>(sample * INT32_MAX);
//       wave[i+1] = wave[i];
//       i += 1;
//     }
// }

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
}

// Triangle wave generated externally
int32_t wave[120] = {
    0,           0,           132800000,   132800000,   265600000,
    265600000,   398400000,   398400000,   531200000,   531200000,
    664000000,   664000000,   796800000,   796800000,   929600000,
    929600000,   1062400000,  1062400000,  1195200000,  1195200000,
    1328000000,  1328000000,  1460800000,  1460800000,  1593600000,
    1593600000,  1726400000,  1726400000,  1859200000,  1859200000,
    1992000000,  1992000000,  1859200000,  1859200000,  1726400000,
    1726400000,  1593600000,  1593600000,  1460800000,  1460800000,
    1328000000,  1328000000,  1195200000,  1195200000,  1062400000,
    1062400000,  929600000,   929600000,   796800000,   796800000,
    664000000,   664000000,   531200000,   531200000,   398400000,
    398400000,   265600000,   265600000,   132800000,   132800000,
    0,           0,           -132800000,  -132800000,  -265600000,
    -265600000,  -398400000,  -398400000,  -531200000,  -531200000,
    -664000000,  -664000000,  -796800000,  -796800000,  -929600000,
    -929600000,  -1062400000, -1062400000, -1195200000, -1195200000,
    -1328000000, -1328000000, -1460800000, -1460800000, -1593600000,
    -1593600000, -1726400000, -1726400000, -1859200000, -1859200000,
    -1992000000, -1992000000, -1859200000, -1859200000, -1726400000,
    -1726400000, -1593600000, -1593600000, -1460800000, -1460800000,
    -1328000000, -1328000000, -1195200000, -1195200000, -1062400000,
    -1062400000, -929600000,  -929600000,  -796800000,  -796800000,
    -664000000,  -664000000,  -531200000,  -531200000,  -398400000,
    -398400000,  -265600000,  -265600000,  -132800000,  -132800000};

int main(void) {
  HAL_Init();
  initGPIO();

  clocks_initialise();
  // 1kHz ticks
  HAL_SYSTICK_Config(SystemCoreClock / 1000);
  HAL_InitTick(1UL << (__NVIC_PRIO_BITS - 1));

  WM8731 codec = WM8731();

  codec.init();

  codec.configureBypass(BYPASS_DISABLE);

  // New initialization code
  SAIDriver newSaiDriver = SAIDriver(true, SAIDriver::BitDepth::BIT_DEPTH_32,
                                     SAIDriver::SampleRate::SAMPLE_RATE_48K);

  // generateSineWave(frequency, amplitude, sampleRate, duration, wave);
  uint8_t *pData = reinterpret_cast<uint8_t *>(wave);

  wave_header parse = wave_header();

  struct wave_header wavHeader;

  unsigned int sample_rate;

  FatFsIntf fs = FatFsIntf();

  FIL fp;

  UINT bRead;

  if (f_open(&fp, "my_wav.wav", FA_READ | FA_OPEN_EXISTING) != FR_OK) {
    __asm__ __volatile__("bkpt #0");
  }
  if (f_read(&fp, buff, 163840, &bRead) != FR_OK) {
    __asm__ __volatile__("bkpt #0");
  }
  // if (newSaiDriver.txTransmit(buff, 163840 / 4, 2000) == 1) {
  //   __asm__ __volatile__("bkpt #0");
  // }

  // read file header
  if (read_wave(fp, wavHeader) != 0) {
    return -1;
  }

  // parse file header, verify that is wave
  if (validate_wave(&wavHeader) != 0) {
    return -1;
  }

  // TODO play sound (from pre-lab 5a)
  play_wave_samples(fp, wavHeader, -1, 0);

  f_close(fp);

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
}