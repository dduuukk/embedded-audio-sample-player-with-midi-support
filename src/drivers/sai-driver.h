#ifndef SAI_DRIVER_H
#define SAI_DRIVER_H

// Include any necessary headers here
#include "stm32h7xx_hal.h"

// Define your class or struct here
class SAIDriver {
    // Declare any member variables or functions here
    public:
        SAIDriver();
        ~SAIDriver();

        void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai);  
        void clocks_initialise(void);
        void initGPIO(void);

    private:
        // GPIO Pin values accoring to the daisy documentation
        enum gpioPinValues {
            SAI1_SA_A = GPIO_PIN_6,
            SAI1_SB_A = GPIO_PIN_3,
            SAI1_FS_A = GPIO_PIN_4,
            SAI1_SCK_A = GPIO_PIN_5,
            SAI1_MCLK_A = GPIO_PIN_2
        };

        void initPins();
        void initClk();
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
        GPIO_InitTypeDef GPIO_Config;
        SAI_HandleTypeDef hsai;
};

// Declare any member variables or functions here

#endif // SAI_DRIVER_H