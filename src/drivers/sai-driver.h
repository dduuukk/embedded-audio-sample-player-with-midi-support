#ifndef SAI_DRIVER_H
#define SAI_DRIVER_H

// Include any necessary headers here
#include "stm32h7xx_hal.h"
#include <cstdint>
#include <stm32h7xx_hal_dma.h>
#include <stm32h7xx_hal_sai.h>

// Define your class or struct here
class SAIDriver {
    // Declare any member variables or functions here
    public:
        SAIDriver(bool useBlockA);
        ~SAIDriver();

        void SAINBTransmit(uint8_t* pData, uint16_t Size, uint32_t Timeout);
        void SAINATransmit(uint8_t* pData, uint16_t Size, uint32_t Timeout);
  
        void clocks_initialise(void);
        void initGPIO(void);
        

    private:
        // GPIO Pin values accoring to the daisy documentation
        

        
        // RCC_ClkInitTypeDef rccClkInstance = {
        //     .ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
        //                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
        //     .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
        //     .AHBCLKDivider = RCC_SYSCLK_DIV1,
        //     .APB1CLKDivider = RCC_HCLK_DIV1,
        //     .APB2CLKDivider = RCC_HCLK_DIV1,
        // };
        // RCC_OscInitTypeDef clock_setup = {
        //     .OscillatorType = RCC_OSCILLATORTYPE_HSE,
        //     .HSEState = RCC_CR_HSEON,
        //     .PLL = {.PLLState = RCC_PLL_ON, .PLLSource = RCC_PLLSOURCE_HSE}
        // };
        
        SAI_HandleTypeDef hsai = {};
        bool useBlockA;
};

enum gpioPinValues {
    SAI1_MCLK_A = GPIO_PIN_2, // SAI1_MCLK_A A2
    SAI1_SB_A = GPIO_PIN_3,
    SAI1_FS_A = GPIO_PIN_4, // SAI1_SCK_A 
    SAI1_SCK_A = GPIO_PIN_5, 
    SAI1_SA_A = GPIO_PIN_6
};

void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai);
void initPins();
void initClk();
void initDMA(SAI_HandleTypeDef* hsai, DMA_HandleTypeDef* hdma);
void deInitDMA(SAI_HandleTypeDef* hsai);

// Declare any member variables or functions here

#endif // SAI_DRIVER_H