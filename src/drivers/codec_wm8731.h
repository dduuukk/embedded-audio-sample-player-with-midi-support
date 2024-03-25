#pragma once
#ifndef CODEC_WM8731_H_
#define CODEC_WM8731_H_

#include "stm32h7xx_hal.h"
#include <stdint.h>

//codec address options
#define ADDR0 0b0011010
#define ADDR1 0b0011011

//register definitions (all are left justified to leave space for the one byte of data also sent)
#define REG_LEFT_LINE_IN    0x00
#define REG_RIGHT_LINE_IN   0x02
#define REG_LEFT_HP_OUT     0x04
#define REG_RIGHT_HP_IN     0x06
#define REG_ANALOG_ROUTING  0x08
#define REG_DIGITAL_ROUTING 0x0A
#define REG_POWER_DOWN_CTRL 0x0C
#define REG_DIGITAL_FORMAT  0x0E
#define REG_SAMPLING_CTRL   0x10
#define REG_ACTIVE          0x12
#define REG_RESET           0x1E

//Line In Controls
#define LINE_IN_0DB_VOL     0x17
#define LINE_IN_MUTE_EN     0x80
#define SIMULTANEOUS_LOAD   0x100

//Headphone Out Controls
#define HP_OUT_0DB_VOL      0x79
#define HP_OUT_MUTE         0x00
#define HP_ZERO_CROSS_EN    0x80
#define SIMULTANEOUS_LOAD   0x100

//Analog Audio Path Controls
#define MICBOOST_ENABLE     0x01
#define MIC_MUTE_ENABLE     0x02
#define ADC_SEL_LINE_INPUT  0x00
#define ADC_SEL_MIC_INPUT   0x04
#define BYPASS_EN           0x08
#define SELECT_DAC          0x10
#define SIDETONE_EN         0x20
#define SIDETONE_ATTN_6DB   0x00
#define SIDETONE_ATTN_9DB   0x40
#define SIDETONE_ATTN_12DB  0x80
#define SIDETONE_ATTN_15DB  0xC0

//Digital Audio Path Controls
#define ADC_HPF_ENABLE      0x00
#define ADC_HPF_DISABLE     0x01
#define NO_DEEMP            0x00
#define DEEMP_32KHZ         0x02
#define DEEMP_44_1KHZ       0x04
#define DEEMP_48KHZ         0x06
#define DAC_SOFTMUTE_ENABLE 0x08
#define CLEAR_DC_OFFSET     0x00
#define STORE_DC_OFFSET     0x10

//Power Down Controls
#define LINE_IN_PWR_DOWN    0x01
#define MIC_IN_PWR_DOWN     0x02
#define ADC_PWR_DOWN        0x04
#define DAC_PWR_DOWN        0x08
#define OUTPUT_PWR_DOWN     0x10
#define OSC_PWR_DOWN        0x20
#define CLKOUT_PWR_DOWN     0x40
#define POWEROFF_EN         0x80

//Digital Audio Interface Format Controls
#define FORMAT_RIGHT_JUST   0x00
#define FORMAT_LEFT_JUST    0x01
#define FORMAT_I2S          0x02
#define FORMAT_DSP_MODE     0x03
#define INPUT_16BITS        0x00
#define INPUT_20BITS        0x04
#define INPUT_24BITS        0x08
#define INPUT_32BITS        0x0B
#define LRP_EN              0x10
#define LRSWAP_EN           0x20
#define SLAVE_MODE_EN       0x00
#define MASTER_MODE_EN      0x40
#define BCLK_NO_INVERT      0x00
#define BCLK_INVERT         0x80

//Sampling Controls              //LOOK AT DATASHEET FOR CCONFIRMING CONTROLS
#define SAMPLE_MODE_NORM    0x00
#define SAMPLE_MODE_USB     0x01
#define BOSR_NORM           0x00
#define BOSR_FAST           0x02
#define ADC_48k_DAC_48k     0x00
#define ADC_48k_DAC_8k      0x04
#define ADC_8k_DAC_48k      0x08
#define ADC_8k_DAC_8k       0x0C
#define ADC_32k_DAC_32k     0x18
#define ADC_96k_DAC_96k     0x1C
#define ADC_44k1_DAC_44k1   0x20
#define ADC_44k1_DAC_8k01   0x24
#define ADC_8k01_DAC_44k1   0x28
#define ADC_8k01_DAC_8k01   0x2C
#define ADC_88k2_DAC_88k2   0x3C
#define CORE_CLOCK_NORM     0x00
#define CORE_CLOCK_DIV2     0x40
#define OUT_CLOCK_NORM      0x00
#define OUT_CLOCK_DIV2      0x80

#define ACTIVE_VALUE        0x01

#define RESET_VALUE         0x00

class WM8731
{
public:
    WM8731();

    void init();

    void enable();
    void disable();
    void reset();

    void registerWrite(uint8_t reg, uint16_t value);

private:
    I2C_HandleTypeDef hi2c2;
    uint8_t dev_address;
    uint8_t reg_address;

};

#endif