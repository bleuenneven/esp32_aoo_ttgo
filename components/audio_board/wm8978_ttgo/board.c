#include "esp_log.h"
#include "audio_mem.h"
#include "board.h"
#include "driver/gpio.h"   


static const char *TAG = "AUDIO_BOARD_TTGO_WM8978";
static audio_board_handle_t board_handle = NULL;

audio_board_handle_t audio_board_init(void)
{
    if (board_handle) {
        ESP_LOGW(TAG, "Board already initialized");
        return board_handle;
    }

    board_handle = (audio_board_handle_t) audio_calloc(1, sizeof(struct audio_board_handle));
    AUDIO_MEM_CHECK(TAG, board_handle, return NULL);

    board_handle->audio_hal = audio_board_codec_init();
    return board_handle;
}

audio_hal_handle_t audio_board_codec_init(void)
{
    audio_hal_codec_config_t audio_codec_cfg = AUDIO_CODEC_DEFAULT_CONFIG();
    audio_hal_handle_t codec_hal = audio_hal_init(&audio_codec_cfg, &AUDIO_CODEC_WM8978_DEFAULT_HANDLE);
    if (!codec_hal) {
        ESP_LOGE(TAG, "Failed to initialize WM8978 codec");
        return NULL;
    }

    int pa_pin = get_pa_enable_gpio();
    if (pa_pin >= 0) {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << pa_pin,
            .pull_down_en = GPIO_PULLDOWN_DISABLE, 
            .pull_up_en = GPIO_PULLUP_DISABLE,    
            .intr_type = GPIO_INTR_DISABLE  
        };
        gpio_config(&io_conf);
        gpio_set_level(pa_pin, 1);
    }

    return codec_hal;
}

audio_board_handle_t audio_board_get_handle(void)
{
    return board_handle;
}

esp_err_t audio_board_deinit(audio_board_handle_t audio_board)
{
    if (!audio_board) return ESP_FAIL;
    esp_err_t ret = audio_hal_deinit(audio_board->audio_hal);
    audio_free(audio_board);
    board_handle = NULL;
    return ret;
}
