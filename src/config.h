#include <freertos/FreeRTOS.h>
#include <driver/i2s.h>

// comment this out if you want to use the internal DAC
#define USE_I2S

// speaker settings - if using I2S
#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_19
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_21
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_18
#define I2S_SPEAKDER_SD_PIN GPIO_NUM_5

// volume control - if required
#define VOLUME_CONTROL ADC1_CHANNEL_7

// button - GPIO 0 is the built in button on most dev boards
#define GPIO_BUTTON GPIO_NUM_0

// i2s speaker pins definition
extern i2s_pin_config_t i2s_speaker_pins;
