#include "sai-driver.h"
#include <stm32h7xx_hal.h>
#include <cmath>
#include <vector>

#define LED_PORT GPIOC
#define LED_PIN GPIO_PIN_0
#define LED_PORT_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE

// Constants for the sine wave
const float frequency = 440.0;  // Frequency in Hz
const float amplitude = 1.0;    // Amplitude (max is 1.0)
const float sampleRate = 44100; // Sample rate in Hz (samples per second)
const float duration = 1.0;     // Duration of the wave in seconds

// Generate the sine wave
std::vector<uint8_t> generateSineWave(float frequency, float amplitude, float sampleRate, float duration) {
    std::vector<uint8_t> wave;
    const float twoPiF = 2.0 * M_PI * frequency;
    const int numSamples = static_cast<int>(duration * sampleRate);
    wave.reserve(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        float t = i / sampleRate;
        float sample = amplitude * sin(twoPiF * t);
        // Normalize to [0, 1] and scale to [0, 255]
        wave.push_back(static_cast<uint8_t>((sample + 1.0f) / 2.0f * 255.0f));
    }
    return wave;
}

static RCC_ClkInitTypeDef rccClkInstance = {
    .ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
    .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
    .AHBCLKDivider = RCC_SYSCLK_DIV1,
    .APB1CLKDivider = RCC_HCLK_DIV1,
    .APB2CLKDivider = RCC_HCLK_DIV1,
};

static RCC_OscInitTypeDef clock_setup = {
    .OscillatorType = RCC_OSCILLATORTYPE_HSE,
    .HSEState = RCC_CR_HSEON,
    .PLL = {.PLLState = RCC_PLL_ON, .PLLSource = RCC_PLLSOURCE_HSE}};

void clocks_initialise(void) {
  HAL_RCC_OscConfig(&clock_setup);
  HAL_RCC_ClockConfig(&rccClkInstance, 5);
  HAL_NVIC_DisableIRQ(SysTick_IRQn);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
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

int main(void) {
  HAL_Init();
  SAIDriver newSAIDriver = SAIDriver();
  initGPIO();

  clocks_initialise();
  // 1kHz ticks
  HAL_SYSTICK_Config(SystemCoreClock / 1000);
  HAL_InitTick(1UL << (__NVIC_PRIO_BITS - 1));

  // uint8_t pData[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  //                      0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
  // newSAIDriver.SAINBTransmit(pData, 16, 50);
  // newSAIDriver.SAINBTransmit(pData, 15, 50);

  std::vector<uint8_t> wave = generateSineWave(frequency, amplitude, sampleRate, duration);
  // Now 'wave' contains 32-bit samples of the 1-second long sine wave

  // Get a pointer to the data
  uint8_t* pData = wave.data();

  // Calculate the number of samples
  int size = static_cast<int>(sampleRate * duration);

  // Pass the data to the SAINBTransmit function
  newSAIDriver.SAINBTransmit(pData, size, 50);


  while (1)
    ;
  return 0;
}

// This prevent name mangling for functions used in C/assembly files.
extern "C" {
void SysTick_Handler(void) {
  // __asm__ __volatile__("bkpt #0");
  HAL_IncTick();

  // 1 Hz blinking
  if ((HAL_GetTick() % 10) == 0) {
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
  }
  HAL_SYSTICK_IRQHandler();
}
}