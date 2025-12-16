#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_codec_dev_os.h"
#include "wm8978_codec.h"
#include "WM8978.h"

#define TAG "WM8978_CODEC"

typedef struct {
    audio_codec_if_t             base;  
    const audio_codec_gpio_if_t  *gpio_if;       
    const audio_codec_ctrl_if_t *ctrl_if;    
    wm8978_dev_t                 dev;        
    esp_codec_dev_sample_info_t  fs;         
    float                        hw_gain;    // Hardware gain
    bool                         is_open;    
    bool                         muted;      
    bool                         enabled;    
} audio_codec_wm8978_t;

/* Volume range mapping for WM8978 (0-63) */
static const esp_codec_dev_vol_range_t vol_range = {
    .min_vol = { .vol = 0, .db_value = 0 },
    .max_vol = { .vol = 63, .db_value = 0 }
};

/* Open codec and initialize WM8978 */
static int wm8978_open(const audio_codec_if_t *h, void *cfg, int cfg_size)
{
    if (!h || !cfg || cfg_size != sizeof(wm8978_codec_cfg_t)) return ESP_CODEC_DEV_INVALID_ARG;

    audio_codec_wm8978_t *codec = (audio_codec_wm8978_t *)h;
    wm8978_codec_cfg_t *codec_cfg = (wm8978_codec_cfg_t *)cfg;

    codec->ctrl_if = codec_cfg->ctrl_if;
    codec->gpio_if = codec_cfg->gpio_if;
    codec->hw_gain = codec_cfg->hw_gain;

    /* Initialize WM8978 device */
    wm8978_init(&codec->dev);
    wm8978_adda_cfg(&codec->dev, 1, 1);      // Enable ADC and DAC
    wm8978_input_cfg(&codec->dev, 1, 0, 0);  // Enable MIC input
    wm8978_output_cfg(&codec->dev, 1, 0);    // Enable HP output
    wm8978_hp_vol_set(&codec->dev, 40, 40);  // Default headphone volume
    wm8978_spk_vol_set(&codec->dev, 40);     // Default speaker volume
    wm8978_spk_boost(&codec->dev, 1);        // Speaker boost

    codec->is_open = true;
    codec->enabled = true;
    codec->muted = false;

    ESP_LOGI(TAG, "WM8978 codec opened");
    return ESP_CODEC_DEV_OK;
}

/* Close codec and put WM8978 to sleep */
static int wm8978_close(const audio_codec_if_t *h)
{
    if (!h) return ESP_CODEC_DEV_INVALID_ARG;
    audio_codec_wm8978_t *codec = (audio_codec_wm8978_t *)h;

    if (codec->is_open) {
        wm8978_sleep(&codec->dev, 1);
        codec->is_open = false;
        codec->enabled = false;
    }

    ESP_LOGI(TAG, "WM8978 codec closed");
    return ESP_CODEC_DEV_OK;
}

/* Enable or disable codec (suspend/resume) */
static int wm8978_enable(const audio_codec_if_t *h, bool enable)
{
    audio_codec_wm8978_t *codec = (audio_codec_wm8978_t *)h;
    if (!codec || !codec->is_open) return ESP_CODEC_DEV_WRONG_STATE;

    if (codec->enabled == enable) return ESP_CODEC_DEV_OK;

    if (enable) {
        wm8978_resume(&codec->dev);
    } else {
        wm8978_sleep(&codec->dev, 1);
    }

    codec->enabled = enable;
    ESP_LOGI(TAG, "WM8978 codec %s", enable ? "enabled" : "disabled");
    return ESP_CODEC_DEV_OK;
}

/* Configure I2S format and sample rate */
static int wm8978_set_fs(const audio_codec_if_t *h, esp_codec_dev_sample_info_t *fs)
{
    audio_codec_wm8978_t *codec = (audio_codec_wm8978_t *)h;
    if (!codec || !codec->is_open || !fs) return ESP_CODEC_DEV_INVALID_ARG;

    codec->fs = *fs;
    wm8978_i2s_cfg(&codec->dev, 2, 0); // Standard I2S, 16-bit
    ESP_LOGI(TAG, "WM8978 I2S configured: rate=%d, bits=%d", fs->sample_rate, fs->bits_per_sample);
    return ESP_CODEC_DEV_OK;
}

/* Set output volume (headphone + speaker) */
static int wm8978_set_vol(const audio_codec_if_t *h, float db_value)
{
    audio_codec_wm8978_t *codec = (audio_codec_wm8978_t *)h;
    if (!codec || !codec->is_open) return ESP_CODEC_DEV_WRONG_STATE;

    int v = (int)db_value;
    if (v < 0) v = 0;
    if (v > 63) v = 63;

    wm8978_hp_vol_set(&codec->dev, v, v);
    wm8978_spk_vol_set(&codec->dev, v);
    ESP_LOGI(TAG, "WM8978 volume set: %d", v);
    return ESP_CODEC_DEV_OK;
}

/* Mute or unmute codec output */
static int wm8978_mute(const audio_codec_if_t *h, bool mute)
{
    audio_codec_wm8978_t *codec = (audio_codec_wm8978_t *)h;
    if (!codec || !codec->is_open) return ESP_CODEC_DEV_WRONG_STATE;

    if (mute) {
        wm8978_hp_vol_set(&codec->dev, 0, 0);
        wm8978_spk_vol_set(&codec->dev, 0);
    } else {
        wm8978_hp_vol_set(&codec->dev, 40, 40);
        wm8978_spk_vol_set(&codec->dev, 40);
    }

    codec->muted = mute;
    ESP_LOGI(TAG, "WM8978 codec %s", mute ? "muted" : "unmuted");
    return ESP_CODEC_DEV_OK;
}

/* Create a new WM8978 codec instance */
const audio_codec_if_t *wm8978_codec_new(wm8978_codec_cfg_t *cfg)
{
    if (!cfg || !cfg->ctrl_if) return NULL;

    audio_codec_wm8978_t *codec = (audio_codec_wm8978_t *)calloc(1, sizeof(audio_codec_wm8978_t));
    if (!codec) return NULL;

    /* Set interface function pointers */
    codec->base.open     = wm8978_open;
    codec->base.close    = wm8978_close;
    codec->base.enable   = wm8978_enable;
    codec->base.set_fs   = wm8978_set_fs;
    codec->base.set_vol  = wm8978_set_vol;
    codec->base.mute     = wm8978_mute;
    codec->base.set_reg  = NULL;
    codec->base.get_reg  = NULL;
    codec->base.dump_reg = NULL;

    /* Open codec immediately */
    if (codec->base.open(&codec->base, cfg, sizeof(wm8978_codec_cfg_t)) != ESP_CODEC_DEV_OK) {
        free(codec);
        return NULL;
    }

    return &codec->base;
}
