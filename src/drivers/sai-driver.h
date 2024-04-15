#ifndef SAI_DRIVER_H
#define SAI_DRIVER_H

// Include any necessary headers here
#include "stm32h7xx_hal.h"
#include <cstdint>
#include <cstring>
#include <stm32h7xx_hal_dma.h>
#include <stm32h7xx_hal_sai.h>

class SAIDriver {
    public:
        enum class BitDepth {
            BIT_DEPTH_16,
            BIT_DEPTH_16_EXTENDED,
            BIT_DEPTH_24,
            BIT_DEPTH_32
        };

        enum class SampleRate {
            SAMPLE_RATE_8K,
            SAMPLE_RATE_16K,
            SAMPLE_RATE_32K,
            SAMPLE_RATE_48K,
            SAMPLE_RATE_96K
        };

        SAIDriver(bool stereo = true, BitDepth bitDepth = BitDepth::BIT_DEPTH_32, SampleRate sampleRate = SampleRate::SAMPLE_RATE_48K);
        ~SAIDriver();

        int txTransmit(uint8_t* pData, uint32_t Size, uint32_t Timeout);

        // static bool dmaQueueFull;

    private:
        SAI_HandleTypeDef hsaiA = {};
        SAI_HandleTypeDef hsaiB = {};
        // bool useBlockA;
        

        // Private member functions
        void genericHSAISetup(SAI_HandleTypeDef* hsai);
        void SAINATransmit(uint8_t* pData, uint16_t Size, uint32_t Timeout);
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
void initClks();
void initDMA(SAI_HandleTypeDef* hsai, DMA_HandleTypeDef* hdma);
void deInitDMA(SAI_HandleTypeDef* hsai);

#endif // SAI_DRIVER_H