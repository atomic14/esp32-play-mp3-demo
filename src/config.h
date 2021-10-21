#include <freertos/FreeRTOS.h>
#include <driver/i2s.h>

// speaker settings
#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_26
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_27
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_25

// i2s speaker pins
extern i2s_pin_config_t i2s_speaker_pins;

// button - GPIO 0 is the built in button on most dev boards
#define GPIO_BUTTON GPIO_NUM_0