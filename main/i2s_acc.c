/*
 * i2s_acc.c
 *
 *  Created on: 2020/09/10
 *      Author: nishi
 *
 *  https://docs.espressif.com/projects/esp-idf/en/v3.3/api-reference/peripherals/i2s.html#_CPPv417i2s_comm_format_t
 */

#include "freertos/FreeRTOS.h"

#include "driver/i2s.h"
#include "esp_log.h"
#include "i2s_acc.h"

static const char *TAG = "I2S_ACC";


static int init_f=0;

void i2s_init(void) {
  // Start listening for audio: MONO @ 16KHz
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
      .sample_rate = 16000,
      .bits_per_sample = (i2s_bits_per_sample_t)16,
      //.bits_per_sample = (i2s_bits_per_sample_t)32,
      //.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,

	  // I2S_COMM_FORMAT_I2S not use by nishi  for deprecated warning
      //.communication_format = I2S_COMM_FORMAT_I2S,
      //.communication_format = I2S_COMM_FORMAT_STAND_I2S,
	  .communication_format = I2S_COMM_FORMAT_PCM,

	  .intr_alloc_flags = 0,
      //.dma_buf_count = 3,
      .dma_buf_count = 6,
      //.dma_buf_len = 300,
      .dma_buf_len = 512,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = -1,
  };
  i2s_pin_config_t pin_config = {
      .bck_io_num = 26,    // IIS_SCLK
      .ws_io_num = 32,     // IIS_LCLK
      .data_out_num = -1,  // IIS_DSIN
      .data_in_num = 33,   // IIS_DOUT
  };
  esp_err_t ret = 0;
  ret = i2s_driver_install((i2s_port_t)1, &i2s_config, 0, NULL);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error in i2s_driver_install");
  }
  ret = i2s_set_pin((i2s_port_t)1, &pin_config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error in i2s_set_pin");
  }

  ret = i2s_zero_dma_buffer((i2s_port_t)1);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error in initializing dma buffer with 0");
  }

  init_f=1;
}

int32_t getSample(uint8_t *dt,int32_t dl){
  size_t bytes_read;
  if(!init_f){
	  i2s_init();
	  init_f=1;
  }
  i2s_read((i2s_port_t)1, (void *)dt, dl,&bytes_read, 10);
  if (bytes_read <= 0 || dl != bytes_read) {
    ESP_LOGE(TAG, "Error in I2S read : %d", bytes_read);
  }
  return bytes_read;

}
