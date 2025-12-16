# ESP32 AOO on LilyGO TTGO (WM8978)

## Gramophone (GRAME) integration

This work is based on the ESP32 **AOO (Audio over OSC)** example project [`esp32_aoo_test`](https://git.iem.at/ressi/esp32_aoo_test) developed by IEM and focuses on adapting the `aoo_receive` example to run on a **LilyGO TTGO board with a WM8978 audio codec**, in the context of the **Gramophone project developed at GRAME** (for more information on the project, see: [https://www.amstramgrame.fr/gramophone/about/](https://www.amstramgrame.fr/gramophone/about/).).

This repository documents the modifications required to make this setup work, including the necessary changes in ESP-ADF configuration and board-specific driver code.

---

## Tested environment

This setup has been tested for now **only** with the following versions:

- ESP-IDF v5.2  
- ESP-ADF v2.7
- AOO library (`develop` branch)

Other versions of ESP-IDF or ESP-ADF may require additional changes.

---

## Overview of the modifications

All modifications were made **inside ESP-ADF**.  
The AOO source code itself was **not modified**.

The following ESP-ADF components were modified or extended:

- `components/audio_hal`
- `components/audio_board`
- `components/esp_codec_dev` (exploratory only, not required for AOO)

---

### Role of each component

#### `audio_hal`
Contains the low-level audio codec drivers.  

The WM8978 driver was added here. It is based on two initial general files [`WM8978.c`](esp-adf/components/audio_hal/driver/WM8978.c) and [`WM8978.h`](esp-adf/components/audio_hal/driver/WM8978.h) written for this codec. These had to be completed in this project to implement the different `audio_hal` functions used in ESP-ADF, with two additional files, [`wm8978_codec.c`](esp-adf/components/audio_hal/driver/wm8978_codec.c) and [`wm8978_codec.h`](esp-adf/components/audio_hal/driver/wm8978_codec.h), written based on the example codes for custom boards and codecs found in [`esp-adf/examples/get-started/play_mp3_control/components/my_board`](https://github.com/espressif/esp-adf/tree/master/examples/get-started/play_mp3_control/components/my_board).

This is the component actually used by AOO for the ESP-ADF audio pipeline.

#### `audio_board`
Describes the hardware board.

A new board definition for the **LilyGO TTGO** was added, based on information about the board found [here](https://github.com/LilyGO/TTGO-TAudio), and following the structure of the example codes in [`esp-adf/examples/get-started/play_mp3_control/components/my_board`](https://github.com/espressif/esp-adf/tree/master/examples/get-started/play_mp3_control/components/my_board).

#### `esp_codec_dev`
Although initially explored, this component is **not used by the ESP-ADF audio pipeline** in this project using **AOO**.  
The `aoo_receive` example uses `audio_hal` to initialize and control the codec.

Even though it is not used, a WM8978 codec implementation was started in [`esp_codec_dev`](https://github.com/espressif/esp_codec_dev), based on the instructions for customization for new codec devices found in its [README](https://github.com/espressif/esp_codec_dev/blob/master/README.md), and sample code [`my_codec.c`](https://github.com/espressif/esp_codec_dev/blob/master/test_apps/codec_dev_test/main/my_codec.c). This implementation uses functions for codec initialization and control that are distinct from the `audio_hal` functions.

---

## Repository structure

This repository mirrors the ESP-ADF directory layout in order to clearly indicate where files were added or modified.

```text
esp-adf/
├── components/
│   ├── audio_hal/
│   │   ├── driver/
│   │   │   └── wm8978/
│   │   │       ├── WM8978.c
│   │   │       ├── WM8978.h
│   │   │       ├── wm8978_codec.c
│   │   │       └── wm8978_codec.h
│   │   └── CMakeLists.txt
│   │
│   ├── audio_board/
│   │   ├── wm8978_ttgo/
│   │   │   ├── board.c
│   │   │   ├── board.h
│   │   │   ├── board_def.h
│   │   │   └── board_pins_config.c
│   │   ├── CMakeLists.txt
│   │   └── Kconfig.projbuild
│   │
│   └── esp_codec_dev/
│   │   ├── device/
│   │   │   └── wm8978/
│   │   │       ├── WM8978.c
│   │   │       ├── WM8978.h
│   │   │       ├── wm8978_codec.c
│   │   │       └── wm8978_codec.h
│   │   ├── CMakeLists.txt
│   │   └── Kconfig
│
└── README.md
```

---

## Audio path at runtime

The audio path used by `aoo_receive` is :
AOO → ESP-ADF pipeline → audio_board → audio_hal → WM8978 → I2S → Speaker

- Codec initialization is done through `audio_hal_init`.
- WM8978 control uses I2C.
- Audio data is transmitted over I2S.

---

## Building and Running `aoo_receive`

The build procedure is identical to the standard ESP32 AOO examples, with the addition of selecting the correct board.
Please refer to the README in [`esp32_aoo_test`](https://git.iem.at/ressi/esp32_aoo_test) for prerequisites and build steps.

Do not forget to configure in `AOO settings` after executing `$ idf.py menuconfig` the mandatory settings:
   - `AOO_WIFI_SSID` (string): WIFI SSID
   - `AOO_WIFI_PASSWORD` (string): WIFI password

### Audio and Board settings

- **Board:** Select `LilyGO TTGO with WM8978 codec` in `Audio HAL` 
- **Audio codec:** The codec WM8978 associated will be used automatically when the LilyGO TTGO board is selected.

