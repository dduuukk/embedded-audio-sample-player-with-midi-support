#include "codec_wm8731.h"

WM8731::WM8731()
{
    I2C_HandleTypeDef _i2c_handle;
    //initialize i2c
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
    if(i2c_.TransmitBlocking(dev_address, buff, 2, 250) != I2CHandle::Result::OK)
    {
        return Result::ERR;
    }
    System::Delay(10);
    return Result::OK;
}