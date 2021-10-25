#pragma once

#include <freertos/FreeRTOS.h>
#include <driver/i2s.h>

/**
 * Base Class for both the DAC and I2S output
 **/
class Output
{
protected:
  i2s_port_t m_i2s_port = I2S_NUM_0;

  int16_t *frames_buffer;
  float volume = 1.0f;

public:
  Output(i2s_port_t i2s_port);
  virtual ~Output();
  virtual void start(int sample_rate) = 0;
  void stop();
  // override this in derived classes to turn the sample into
  // something the output device expects - for the default case
  // this is simply a pass through
  virtual uint16_t process_sample(int16_t sample) { return sample; }
  // NOTE - a frame consists of both a left and a right sample
  void write(int16_t *samples, int frames);
  // set the volume between 0 and 4096
  void set_volume(float volume)
  {
    this->volume = volume;
  }
};
