#include <stm32h7xx_hal.h>

#include "codec_wm8731.h"

#define LED_PORT GPIOC
#define LED_PIN GPIO_PIN_0
#define LED_PORT_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE

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
  initGPIO();

  clocks_initialise();
  // 1kHz ticks
  HAL_SYSTICK_Config(SystemCoreClock / 1000);
  HAL_InitTick(1UL << (__NVIC_PRIO_BITS - 1));

  WM8731 codec = WM8731();

  codec.init();

  codec.registerWrite(
      REG_ANALOG_ROUTING,
    MIC_MUTE_ENABLE | ADC_SEL_LINE_INPUT | SELECT_DAC | BYPASS_EN); // same config as the init, but enables the analog bypass

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