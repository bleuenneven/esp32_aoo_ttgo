#include "driver/gpio.h"
#include "board.h"
#include "audio_error.h"
#include "esp_log.h"
#include "driver/i2c.h"           
#include "driver/i2s_std.h"        


static const char *TAG = "TTGO_WM8978";

esp_err_t get_i2c_pins(i2c_port_t port, i2c_config_t *i2c_config)
{
    if (!i2c_config) return ESP_FAIL;
    if (port == I2C_NUM_0) {
        i2c_config->sda_io_num = CODEC_I2C_SDA;
        i2c_config->scl_io_num = CODEC_I2C_SCL;
    } else {
        ESP_LOGE(TAG, "Unsupported I2C port");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t get_i2s_pins(int port, board_i2s_pin_t *i2s_config)
{
    if (!i2s_config) return ESP_FAIL;
    if (port == CODEC_I2S_PORT) {
        i2s_config->bck_io_num = CODEC_I2S_BCLK;
        i2s_config->ws_io_num = CODEC_I2S_LRCK;
        i2s_config->data_out_num = CODEC_I2S_DOUT;
        i2s_config->data_in_num = CODEC_I2S_DIN;
        i2s_config->mck_io_num = CODEC_I2S_MCLK;
        return ESP_OK;
    }
    ESP_LOGE(TAG, "Unsupported I2S port");
    return ESP_FAIL;
}

int8_t get_pa_enable_gpio(void)
{
    return PA_ENABLE_GPIO;
}
