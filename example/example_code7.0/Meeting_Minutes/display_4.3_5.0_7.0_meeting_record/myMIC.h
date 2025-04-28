#include <driver/i2s.h>


#define SAMPLE_RATE 16000  // Sampling rate
#define SAMPLE_SIZE 1024   // Number of samples per transfer
#define I2S_MIC_NUM I2S_NUM_0




const i2s_config_t i2s_config_mic = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),  // Master mode and receive mode
  .sample_rate = SAMPLE_RATE,                           // Sampling rate
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,         // Number of bits per sample
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,          // Only use the left channel
  .communication_format = I2S_COMM_FORMAT_I2S,          // Communication format
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,             // Interrupt allocation flag
  .dma_buf_count = 8,                                   // Number of DMA buffers
  .dma_buf_len = SAMPLE_SIZE,                           // Length of DMA buffer
  .use_apll = false,                                    // Do not use APLL
  .tx_desc_auto_clear = false,                          // Do not automatically clear transmit descriptors
  .fixed_mclk = 0                                       // Fixed MCLK frequency
};

// I2S microphone pin configuration structure
const i2s_pin_config_t pin_config_mic = {
  .bck_io_num = 19,                   // BCK pin number,For 2.4-inch, 2.8-inch, and 3.5-inch devices, it is 9
  .ws_io_num = 2,                     // LRCL pin number,For 2.4-inch, 2.8-inch, and 3.5-inch devices, it is 3
  .data_out_num = I2S_PIN_NO_CHANGE,  // Data output pin remains unchanged
  .data_in_num = 20                   // Data input pin number,For 2.4-inch, 2.8-inch, and 3.5-inch devices, it is 10
};