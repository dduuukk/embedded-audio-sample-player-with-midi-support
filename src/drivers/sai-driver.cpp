#include "sai-driver.h"
#include "Legacy/stm32_hal_legacy.h"
#include "stm32h750xx.h"
#include "stm32h7xx_hal_def.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_sai.h"

// Initializer
SAIDriver::SAIDriver() {
    // Set SAI instance
    hsai.Instance = SAI1_Block_A;

    // Set up HSAI configurations such as protocol and such
    // hsai->Init.request = SAI_BLOCKA_REQUEST_ENABLE;
    // hsai->Init.direction = SAI_DIR_TX;
    hsai.Init.AudioMode = SAI_MODEMASTER_TX;

    hsai.Init.Synchro = SAI_ASYNCHRONOUS; // Master mode should be asynchronous
    hsai.Init.SynchroExt = SAI_SYNCEXT_DISABLE;

    // TODO: CHECK IF NOT DEFINING MCKOUTPUT IS OK
    // hsai->Init.MckOutput = SAI_MCK_OUTPUT_DISABLE;

    hsai.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLED;
    hsai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
    hsai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
    hsai.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
    // TODO: CHECK IF NOT DEFINING THIS IS OK
    // hsai->Init.Mckdiv = 0;
    // TODO: OVERSAMPLING
    
    hsai.Init.MonoStereoMode = SAI_STEREOMODE;
    hsai.Init.CompandingMode = SAI_NOCOMPANDING;
    hsai.Init.TriState = SAI_OUTPUT_NOTRELEASED;

    hsai.Init.Protocol = SAI_I2S_STANDARD;
    hsai.Init.DataSize = SAI_DATASIZE_32;
    // hsai->Init.FirstBit = SAI_FIRSTBIT_MSB;

    // Perform any necessary initialization for SAI peripheral
    // Simplified mode initialization
    // TODO: ADD FUNCTIONS TO CHANGE PROTOCOL AND BIT DEPTH ON STARTUP
    if(HAL_SAI_InitProtocol(&hsai, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_32BIT, 2) != HAL_OK) {
        // TODO: SOME SORT OF ERROR HANDLING
        __asm__ __volatile__("bkpt #0");
    }
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

        // Enable SAI interrupts
        // TODO: CHECK IF THIS IS NECESSARY FOR THE FIRST ITERATION
        // HAL_NVIC_SetPriority(SAI1_IRQn, 0, 0);
        // HAL_NVIC_EnableIRQ(SAI1_IRQn);

        // Skipping DMA configuration for now
        // Otherwise, DMA configuration goes here
        
    }
}

// HAL SAI MSP DEINIT
SAIDriver::~SAIDriver() {
    // Perform any necessary deinitialization for SAI peripheral
    HAL_GPIO_DeInit(GPIOE, SAI1_SA_A);
    HAL_GPIO_DeInit(GPIOE, SAI1_SB_A);
    HAL_GPIO_DeInit(GPIOE, SAI1_FS_A);
    HAL_GPIO_DeInit(GPIOE, SAI1_SCK_A);
    HAL_GPIO_DeInit(GPIOE, SAI1_MCLK_A);
    __HAL_RCC_SAI1_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOG_CLK_DISABLE();
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_DMA1_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(SAI1_IRQn);
    HAL_SAI_DeInit(&hsai);
}