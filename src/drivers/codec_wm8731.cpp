#include "codec_wm8731.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_rcc.h"

extern "C" void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hi2c->Instance==I2C2)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        /**I2C2 GPIO Configuration
        PH4      ------> I2C2_SCL
        PB11     ------> I2C2_SDA
        */
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull  = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;

        GPIO_InitStruct.Pin = GPIO_PIN_4;
        HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        //__HAL_RCC_DMA1_CLK_ENABLE(); //from Daisy

        // HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0); //from Daisy
        // HAL_NVIC_EnableIRQ(I2C2_EV_IRQn); //from Daisy
     }
}

WM8731::WM8731()
{
    hi2c2.Instance = I2C2;
    if(HAL_RCC_GetPCLK1Freq() == 120000000) //from Daisy
    {
        hi2c2.Init.Timing = 0x6090435F;
    }
    else
    {
        hi2c2.Init.Timing = 0x30E0628A;
    }
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    __HAL_RCC_I2C1_CLK_ENABLE();

    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        __asm__ __volatile__("bkpt #0");
    }
}

void WM8731::init()
{
    reset();

    //go through core list of initial setup
    //Left line in control, copies to right line in
    registerWrite(REG_LEFT_LINE_IN, LINE_IN_0DB_VOL | SIMULTANEOUS_LOAD);

    //Left headphone out control, copies to right headphone out
    registerWrite(REG_LEFT_HP_OUT, HP_OUT_MUTE | SIMULTANEOUS_LOAD);

    //Analog path control
    registerWrite(REG_ANALOG_ROUTING, MIC_MUTE_ENABLE | ADC_SEL_LINE_INPUT | SELECT_DAC);

    //Digital path control
    registerWrite(REG_DIGITAL_ROUTING, NO_DEEMP);

    //Power down control
    registerWrite(REG_POWER_DOWN_CTRL, MIC_IN_PWR_DOWN | OSC_PWR_DOWN | CLKOUT_PWR_DOWN);

    //Digital audio interface format control
    registerWrite(REG_DIGITAL_FORMAT, FORMAT_I2S); //LOOK INTO THIS ONE

    //Sampling control
    registerWrite(REG_SAMPLING_CTRL, SAMPLE_MODE_NORM | BOSR_NORM | ADC_48k_DAC_48k);

    enable();
}

void WM8731::enable()
{
    registerWrite(REG_ACTIVE, ACTIVE_VALUE);
}

void WM8731::disable()
{
    registerWrite(REG_ACTIVE, 0);
}

void WM8731::reset()
{
    registerWrite(REG_RESET, RESET_VALUE);
}

void WM8731::registerWrite(uint8_t reg, uint16_t data)
{
    uint8_t byte_1  = reg | ((data >> 8) & 0x01);
    uint8_t byte_2  = data & 0xff;
    uint8_t buff[2] = {byte_1, byte_2};
    if(HAL_I2C_Master_Transmit(&hi2c2, ADDR0, buff, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        __asm__ __volatile__("bkpt #0");
    }
    HAL_Delay(10);
}