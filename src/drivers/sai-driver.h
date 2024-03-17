#ifndef SAI_DRIVER_H
#define SAI_DRIVER_H

// Include any necessary headers here
#include "stm32h7xx_hal.h"

// Define your class or struct here
class SAIDriver {
    // Declare any member variables or functions here
    public:
        SAIDriver(SAI_HandleTypeDef *hsai);
        ~SAIDriver();

        void HAL_SAI_MspInit(SAI_HandleTypeDef* hsai);  
        void clocks_initialise(void);
        void initGPIO(void);
        // {
        //     // Perform any necessary initialization for SAI peripheral
        //     if (hsai->Instance == /* your SAI peripheral instance */) {
        //         // Enable SAI clock
        //         /* your code here */

        //         // Configure SAI pins
        //         /* your code here */

        //         // Configure SAI interrupts
        //         /* your code here */
        //     }
        // }
    private:
        RCC_ClkInitTypeDef rccClkInstance = {
            .ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                        RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
            .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
            .AHBCLKDivider = RCC_SYSCLK_DIV1,
            .APB1CLKDivider = RCC_HCLK_DIV1,
            .APB2CLKDivider = RCC_HCLK_DIV1,
        };
        RCC_OscInitTypeDef clock_setup = {
            .OscillatorType = RCC_OSCILLATORTYPE_HSE,
            .HSEState = RCC_CR_HSEON,
            .PLL = {.PLLState = RCC_PLL_ON, .PLLSource = RCC_PLLSOURCE_HSE}
        };
        GPIO_InitTypeDef GPIO_Config;
};

// Declare any member variables or functions here

#endif // SAI_DRIVER_H