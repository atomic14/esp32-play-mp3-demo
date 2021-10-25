#include <SPIFFS.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <config.h>
#include <I2SOutput.h>
#include <DACOutput.h>
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#define MINIMP3_NO_STDIO
#include "minimp3.h"

extern "C"
{
  void app_main();
}

void wait_for_button_push()
{
  while (gpio_get_level(GPIO_BUTTON) == 1)
  {
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

const int BUFFER_SIZE = 1024;

void play_task(void *param)
{
#ifdef VOLUME_CONTROL
  // set up the ADC for reading the volume control
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
#endif
  // create the output - see config.h for settings
#ifdef USE_I2S
  Output *output = new I2SOutput(I2S_NUM_0, i2s_speaker_pins);
#else
  Output *output = new DACOutput();
#endif
#ifdef I2S_SPEAKDER_SD_PIN
  // if you I2S amp has a SD pin, you'll need to turn it on
  gpio_set_direction(I2S_SPEAKDER_SD_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(I2S_SPEAKDER_SD_PIN, 1);
#endif
  // setup the button to trigger playback - see config.h for settings
  gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
  gpio_set_pull_mode(GPIO_BUTTON, GPIO_PULLUP_ONLY);
  // create the file system
  SPIFFS spiffs("/fs");
  // setup for the mp3 decoded
  short *pcm = (short *)malloc(sizeof(short) * MINIMP3_MAX_SAMPLES_PER_FRAME);
  uint8_t *input_buf = (uint8_t *)malloc(BUFFER_SIZE);
  if (!pcm)
  {
    ESP_LOGE("main", "Failed to allocate pcm memory");
  }
  if (!input_buf)
  {
    ESP_LOGE("main", "Failed to allocate input_buf memory");
  }
  while (true)
  {
    // wait for the button to be pushed
    wait_for_button_push();
    // mp3 decoder state
    mp3dec_t mp3d = {};
    mp3dec_init(&mp3d);
    mp3dec_frame_info_t info = {};
    // keep track of how much data we have buffered, need to read and decoded
    int to_read = BUFFER_SIZE;
    int buffered = 0;
    int decoded = 0;
    bool is_output_started = false;
    // this assumes that you have uploaded the mp3 file to the SPIFFS
    FILE *fp = fopen("/fs/test.mp3", "r");
    if (!fp)
    {
      ESP_LOGE("main", "Failed to open file");
      continue;
    }
    while (1)
    {
#ifdef VOLUME_CONTROL
      auto adc_value = float(adc1_get_raw(VOLUME_CONTROL)) / 4096.0f;
      // make the actual volume match how people hear
      // https://ux.stackexchange.com/questions/79672/why-dont-commercial-products-use-logarithmic-volume-controls
      output->set_volume(adc_value * adc_value);
#endif
      // read in the data that is needed to top up the buffer
      size_t n = fread(input_buf + buffered, 1, to_read, fp);
      // feed the watchdog
      vTaskDelay(pdMS_TO_TICKS(1));
      // ESP_LOGI("main", "Read %d bytes\n", n);
      buffered += n;
      if (buffered == 0)
      {
        // we've reached the end of the file and processed all the buffered data
        output->stop();
        is_output_started = false;
        break;
      }
      // decode the next frame
      int samples = mp3dec_decode_frame(&mp3d, input_buf, buffered, pcm, &info);
      // we've processed this may bytes from teh buffered data
      buffered -= info.frame_bytes;
      // shift the remaining data to the front of the buffer
      memmove(input_buf, input_buf + info.frame_bytes, buffered);
      // we need to top up the buffer from the file
      to_read = info.frame_bytes;
      if (samples > 0)
      {
        // if we haven't started the output yet we can do it now as we now know the sample rate and number of channels
        if (!is_output_started)
        {
          output->start(info.hz);
          is_output_started = true;
        }
        // if we've decoded a frame of mono samples convert it to stereo by duplicating the left channel
        // we can do this in place as our samples buffer has enough space
        if (info.channels == 1)
        {
          for (int i = samples - 1; i >= 0; i--)
          {
            pcm[i * 2] = pcm[i];
            pcm[i * 2 - 1] = pcm[i];
          }
        }
        // write the decoded samples to the I2S output
        output->write(pcm, samples);
        // keep track of how many samples we've decoded
        decoded += samples;
      }
      // ESP_LOGI("main", "decoded %d samples\n", decoded);
    }
    ESP_LOGI("main", "Finished\n");
    fclose(fp);
  }
}

void app_main()
{
  xTaskCreatePinnedToCore(play_task, "task", 32768, NULL, 1, NULL, 1);
}