#ifndef _AUDIO_BOARD_DEFINITION_H_
#define _AUDIO_BOARD_DEFINITION_H_

/**
 * @brief Audio Codec WM8978 configuration
 */
#define FUNC_AUDIO_CODEC_EN         (1)
#define WM8978_MCLK_SOURCE          (0)
#define PA_ENABLE_GPIO              -1

#define CODEC_I2C_SDA               GPIO_NUM_19
#define CODEC_I2C_SCL               GPIO_NUM_18

#define CODEC_I2S_PORT              ((i2s_port_t)0)
#define CODEC_I2S_BCLK              GPIO_NUM_33
#define CODEC_I2S_LRCK              GPIO_NUM_25
#define CODEC_I2S_DOUT              GPIO_NUM_26
#define CODEC_I2S_DIN               GPIO_NUM_27
#define CODEC_I2S_MCLK              -1

#define CODEC_BITS_PER_SAMPLE       (16)
#define CODEC_SAMPLE_RATE           (48000)

#define BOARD_PA_GAIN_DB            (10)

/**
 * @brief SDCARD Function Definition (not compiling if not defined)
 */
#define FUNC_SDCARD_EN             (0)
#define SDCARD_OPEN_FILE_NUM_MAX    5
#define SDCARD_INTR_GPIO            -1
#define SDCARD_PWR_CTRL             -1

#define ESP_SD_PIN_CLK              -1
#define ESP_SD_PIN_CMD              -1
#define ESP_SD_PIN_D0               -1
#define ESP_SD_PIN_D3               -1


/**
 * @brief Disable unused board features
 */
#define FUNC_BUTTON_EN              (0)   
#define FUNC_SYS_LEN_EN             (0)   
#define FUNC_BATTERY_DET_EN         (0)   


/**
 * @brief Default audio HAL configuration
 */
#define AUDIO_CODEC_DEFAULT_CONFIG(){                   \
        .adc_input  = AUDIO_HAL_ADC_INPUT_LINE1,        \
        .dac_output = AUDIO_HAL_DAC_OUTPUT_ALL,         \
        .codec_mode = AUDIO_HAL_CODEC_MODE_BOTH,        \
        .i2s_iface = {                                  \
            .mode = AUDIO_HAL_MODE_SLAVE,               \
            .fmt = AUDIO_HAL_I2S_NORMAL,                \
            .samples = AUDIO_HAL_48K_SAMPLES,           \
            .bits = AUDIO_HAL_BIT_LENGTH_16BITS,        \
        },                                              \
}

#ifdef __cplusplus
extern "C" {
#endif

extern audio_hal_func_t AUDIO_CODEC_WM8978_DEFAULT_HANDLE;

#ifdef __cplusplus
}
#endif


#endif


