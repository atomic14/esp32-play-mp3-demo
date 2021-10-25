
#include "Output.h"
#include <esp_log.h>
#include <driver/i2s.h>

static const char *TAG = "OUT";

// number of frames to try and send at once (a frame is a left and right sample)
const int NUM_FRAMES_TO_SEND = 256;

Output::Output(i2s_port_t i2s_port) : m_i2s_port(i2s_port)
{
  frames_buffer = (int16_t *)malloc(2 * sizeof(int16_t) * NUM_FRAMES_TO_SEND);
}

Output::~Output()
{
  free(frames_buffer);
}

void Output::stop()
{
  // stop the i2S driver
  i2s_stop(m_i2s_port);
  i2s_driver_uninstall(m_i2s_port);
}

void Output::write(int16_t *samples, int frames)
{
  // this will contain the prepared samples for sending to the I2S device
  int frame_index = 0;
  while (frame_index < frames)
  {
    // fill up the frames buffer with the next NUM_FRAMES_TO_SEND frames
    int frames_to_send = 0;
    for (int i = 0; i < NUM_FRAMES_TO_SEND && frame_index < frames; i++)
    {
      int left_sample = process_sample(volume * float(samples[frame_index * 2]));
      int right_sample = process_sample(volume * float(samples[frame_index * 2 + 1]));
      frames_buffer[i * 2] = left_sample;
      frames_buffer[i * 2 + 1] = right_sample;
      frames_to_send++;
      frame_index++;
    }
    // write data to the i2s peripheral - this will block until the data is sent
    size_t bytes_written = 0;
    i2s_write(m_i2s_port, frames_buffer, frames_to_send * sizeof(int16_t) * 2, &bytes_written, portMAX_DELAY);
    if (bytes_written != frames_to_send * sizeof(int16_t) * 2)
    {
      ESP_LOGE(TAG, "Did not write all bytes");
    }
  }
}
