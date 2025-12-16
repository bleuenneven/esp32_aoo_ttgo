#ifndef _WM8978_CODEC_H_
#define _WM8978_CODEC_H_

#include "audio_codec_if.h"
#include "audio_codec_ctrl_if.h"
#include "audio_codec_gpio_if.h"
#include "esp_codec_dev_vol.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM8978_CODEC_DEFAULT_ADDR (0x1A)

/**
 * @brief WM8978 codec configuration
 */
typedef struct {
    const audio_codec_ctrl_if_t *ctrl_if;  /*!< Codec control interface */
    const audio_codec_gpio_if_t *gpio_if;  /*!< GPIO interface for control */
    float                        hw_gain;  /*!< Hardware gain*/
} wm8978_codec_cfg_t;

/**
 * @brief Create a new WM8978 codec interface
 *
 * @param codec_cfg WM8978 codec configuration
 *
 * @return
 *       - NULL    Not enough memory or codec failed to open
 *       - Others  WM8978 codec interface
 */
const audio_codec_if_t *wm8978_codec_new(wm8978_codec_cfg_t *codec_cfg);

#ifdef __cplusplus
}
#endif

#endif /* _WM8978_CODEC_H_ */
