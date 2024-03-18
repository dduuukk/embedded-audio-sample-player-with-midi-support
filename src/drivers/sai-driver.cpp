#include "sai-driver.h"

// Initializer
SAIDriver::SAIDriver(SAI_HandleTypeDef *hsai) {
    // Perform any necessary initialization for SAI peripheral
    HAL_SAI_MspInit(hsai);
}

// Clock source init
void SAIDriver::initClk() {
    // Enable all possible necessary clocks possible for daisy
    // Enable SAI interface clocks
    __HAL_RCC_SAI1_CLK_ENABLE();

    // Enable DMA clocks
    // NOTE: NOT SURE THIS IS NECESSARY RIGHT NOW, WILL BE IN FUTURE
    __HAL_RCC_DMA1_CLK_ENABLE();

    // Enable clocks for SAI GPIOs
    // TODO: DEPENDING ON WHERE WE INPUT/OUTPUT FOR TESTING CHANGE THIS
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
}

// SAI pins init
void SAIDriver::initPins() {
    // Initialize pins for SAI1
    GPIO_Config.Pin = SAIDriver::gpioPinValues::SAI1_FS_A |
                      SAIDriver::gpioPinValues::SAI1_MCLK_A |
                      SAIDriver::gpioPinValues::SAI1_SCK_A |
                      SAIDriver::gpioPinValues::SAI1_SA_A |
                      SAIDriver::gpioPinValues::SAI1_SB_A;
    // Configure SAI pins as alternate function pull-up.
    GPIO_Config.Mode = GPIO_MODE_AF_PP;
    GPIO_Config.Pull = GPIO_PULLUP;
    GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH; // TODO: CHECK OVER SPEEDS, SEE IF MEDIUM IS OK
    GPIO_Config.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &GPIO_Config);
}

// HAL SAI MSP Init
void SAIDriver::HAL_SAI_MspInit(SAI_HandleTypeDef* hsai) {
    // Perform any necessary initialization for SAI peripheral
    if (hsai->Instance == SAI1_Block_A || hsai->Instance == SAI1_Block_B) {
        // Enable clocks
        initClk();

        // Configure SAI pins
        initPins();
    }
}