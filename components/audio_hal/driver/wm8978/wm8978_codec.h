#pragma once
#include "audio_hal.h"
#include "WM8978.h"


esp_err_t wm8978_codec_init(audio_hal_codec_config_t *cfg);
esp_err_t wm8978_codec_deinit(void);
esp_err_t wm8978_codec_ctrl_state(audio_hal_codec_mode_t mode, audio_hal_ctrl_t ctrl_state);
esp_err_t wm8978_codec_config_i2s(audio_hal_codec_mode_t mode, audio_hal_codec_i2s_iface_t *iface);
esp_err_t wm8978_codec_set_volume(int vol);
esp_err_t wm8978_codec_get_volume(int *vol);
esp_err_t wm8978_codec_set_mute(bool mute);

