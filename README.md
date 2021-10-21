![build status](https://github.com/atomic14/esp32-play-mp3-demo/actions/workflows/build-test-on-push.yml/badge.svg)

# ESP32 MP3 Player

This repo contains a simple demonstration of how to play an MP3 file on the ESP32.

You can configure the output to be either an I2S device or the built-in DAC to output the audio to a set of headphones or an analogue amplifier.

Be aware that the DAC is extremely noisy so at low vollume levels most of the sound will be dominated by noise. It may be possible to reduce the noise by using a low pass filter on the output.

In theory you should have a DC blocking capacitor between the output and headphones, but I found this to cause a large amount of noise.

When using the DAC the left channel will be output on GPIO26 and the right channel will be output on GPIO25.

I've added an optional volume control. You can connect a potentiometer between 3.3v and ground and connect it to one of the ADC channels.

Make sure you upload the filesystem as the sample mp3 file is stored on SPIFFS.

## Configuration

All the configuration settings are in the `config.h` file.

```c
// comment this line out if you want to use the built-in DAC
#define USE_I2S

// speaker settings - if using I2S
#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_19
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_21
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_18
#define I2S_SPEAKDER_SD_PIN GPIO_NUM_5

// volume control - if required - comment this out if you don't want it
#define VOLUME_CONTROL ADC1_CHANNEL_7

// button - GPIO 0 is the built in button on most dev boards
#define GPIO_BUTTON GPIO_NUM_0
```
