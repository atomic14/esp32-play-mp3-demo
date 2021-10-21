#pragma once

#include "Output.h"

/**
 * Base Class for both the ADC and I2S sampler
 **/
class DACOutput : public Output
{
public:
    // DAC can only be used with I2S_NUM_0
    DACOutput() : Output(I2S_NUM_0){};
    void start(int sample_rate);
    virtual uint16_t process_sample(int16_t sample)
    {
        //return sample;
        int32_t raw = sample;
        // DAC needs unsigned 16 bit samples
        return (uint16_t)(raw + 32768);
    }
};
