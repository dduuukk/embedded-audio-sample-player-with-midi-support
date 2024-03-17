#include "sai-driver.h"

// Initializer
SAIDriver::SAIDriver(SAI_HandleTypeDef *hsai) {
    // Perform any necessary initialization for SAI peripheral
    HAL_SAI_MspInit(SAI_HandleTypeDef *hsai);
}

// HAL SAI MSP Init
void SAIDriver::HAL_SAI_MspInit(SAI_HandleTypeDef* hsai) {
    // Perform any necessary initialization for SAI peripheral
    if (hsai->Instance == SAI1_Block_A) {
        // Enable SAI interface clock
        __HAL_RCC_SAI1_CLK_ENABLE();


        // __HAL_RCC_SAI1_CLK_ENABLE();

        // Configure SAI pins
        // GPIO_Config.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        // GPIO_Config.Mode = GPIO_MODE_AF_PP;
        // GPIO_Config.Pull = GPIO_NOPULL;
        // GPIO_Config.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        // GPIO_Config.Alternate = GPIO_AF6_SAI1;
        // HAL_GPIO_Init(GPIOE, &GPIO_Config);
    }
}