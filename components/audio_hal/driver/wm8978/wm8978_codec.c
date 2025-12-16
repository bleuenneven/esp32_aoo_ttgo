#include "wm8978_codec.h"
#include "WM8978.h"
#include "esp_log.h"
#include "board.h"
#include "board_def.h"

static const char *TAG = "WM8978_CODEC";
static wm8978_dev_t codec_dev;
static bool codec_initialized = false;

esp_err_t wm8978_codec_init(audio_hal_codec_config_t *cfg)
{
    if (codec_initialized) {
        ESP_LOGW(TAG, "WM8978 codec already initialized");
        return ESP_OK;
    }
    wm8978_init(&codec_dev);
    wm8978_adda_cfg(&codec_dev, 1, 1);      // Enable ADC and DAC
    wm8978_input_cfg(&codec_dev, 1, 0, 0);  // Enable MIC
    wm8978_output_cfg(&codec_dev, 1, 0);    // Enable HP, disable bypass
    wm8978_hp_vol_set(&codec_dev, 40, 40);  // Initial volumes
    wm8978_spk_vol_set(&codec_dev, 40);
    wm8978_spk_boost(&codec_dev, 1);
    codec_initialized = true;
    ESP_LOGI(TAG, "WM8978 codec initialized");
    return ESP_OK;
}

esp_err_t wm8978_codec_deinit(void)
{
    if (!codec_initialized) return ESP_OK;
    wm8978_sleep(&codec_dev, 1);
    codec_initialized = false;
    ESP_LOGI(TAG, "WM8978 codec deinitialized");
    return ESP_OK;
}

esp_err_t wm8978_codec_ctrl_state(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state)
{
    if (!codec_initialized) return ESP_FAIL;
    if (ctrl_state == AUDIO_HAL_CTRL_START) {
        wm8978_resume(&codec_dev);
    } else {
        wm8978_sleep(&codec_dev, 1);
    }
    ESP_LOGI(TAG, "WM8978 ctrl_state: mode=%d, state=%d", mode, ctrl_state);
    return ESP_OK;
}

esp_err_t wm8978_codec_config_i2s(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t *iface)
{
    if (!codec_initialized) return ESP_FAIL;
    wm8978_i2s_cfg(&codec_dev, 2, 0); // standard I2S, 16-bit
    ESP_LOGI(TAG, "WM8978 I2S configured");
    return ESP_OK;
}

esp_err_t wm8978_codec_set_volume(int vol)
{
    if (!codec_initialized) return ESP_FAIL;
    uint8_t v = (vol < 0) ? 0 : ((vol > 63) ? 63 : vol);
    wm8978_hp_vol_set(&codec_dev, v, v);
    wm8978_spk_vol_set(&codec_dev, v);
    ESP_LOGI(TAG, "WM8978 volume set to %d", v);
    return ESP_OK;
}

esp_err_t wm8978_codec_get_volume(int *vol)
{
    if (!codec_initialized || !vol) return ESP_FAIL;
    *vol = 0; 
    return ESP_OK;
}

esp_err_t wm8978_codec_set_mute(bool mute)
{
    if (!codec_initialized) return ESP_FAIL;
    if (mute) {
        wm8978_spk_vol_set(&codec_dev, 0);       
        wm8978_hp_vol_set(&codec_dev, 0, 0);    
    } else {
        wm8978_spk_vol_set(&codec_dev, 40);      
        wm8978_hp_vol_set(&codec_dev, 40, 40);
    }
    ESP_LOGI(TAG, "WM8978 %s", mute ? "muted" : "unmuted");
    return ESP_OK;
}

// HAL default handle
audio_hal_func_t AUDIO_CODEC_WM8978_DEFAULT_HANDLE = {
    .audio_codec_initialize   = wm8978_codec_init,
    .audio_codec_deinitialize = wm8978_codec_deinit,
    .audio_codec_ctrl         = wm8978_codec_ctrl_state,
    .audio_codec_config_iface = wm8978_codec_config_i2s,
    .audio_codec_set_mute     = wm8978_codec_set_mute,
    .audio_codec_set_volume   = wm8978_codec_set_volume,
    .audio_codec_get_volume   = wm8978_codec_get_volume,
    .audio_codec_enable_pa    = NULL,
    .audio_hal_lock           = NULL,
    .handle                   = NULL,
};
