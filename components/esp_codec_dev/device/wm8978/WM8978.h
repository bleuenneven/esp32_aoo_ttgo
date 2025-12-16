/*******************************************************************************
 I2C Configuration for a Wolfson WM8978 Audio Codec.
 This file was written based on the information provided in the Codec Specs
 and on the NuovotonDuino project: https://github.com/DFRobot/NuvotonDuino.
 *******************************************************************************/

#ifndef __WM8978_H
#define __WM8978_H

#include <stdio.h>
#include <stdint.h>

#define I2C_MASTER_NUM 1 /*!< I2C port number for master dev */
#define I2C_MASTER_SCL_IO 18
#define I2C_MASTER_SDA_IO 19
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

#define WM8978_ADDR 0X1A    //WM8978

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define ACK_CHECK_EN 0x1

#define EQ1_80Hz 0X00
#define EQ1_105Hz 0X01
#define EQ1_135Hz 0X02
#define EQ1_175Hz    0X03

#define EQ2_230Hz    0X00
#define EQ2_300Hz    0X01
#define EQ2_385Hz    0X02
#define EQ2_500Hz    0X03

#define EQ3_650Hz    0X00
#define EQ3_850Hz    0X01
#define EQ3_1100Hz 0X02
#define EQ3_14000Hz 0X03

#define EQ4_1800Hz 0X00
#define EQ4_2400Hz 0X01
#define EQ4_3200Hz 0X02
#define EQ4_4100Hz 0X03

#define EQ5_5300Hz 0X00
#define EQ5_6900Hz 0X01
#define EQ5_9000Hz 0X02
#define EQ5_11700Hz 0X03

typedef struct {
    uint16_t regval_tbl[58];
} wm8978_dev_t;


uint8_t wm8978_init(wm8978_dev_t *dev);
void wm8978_init_i2c(void);
uint8_t wm8978_write_reg(wm8978_dev_t *dev,uint8_t reg,uint16_t val);
uint16_t wm8978_read_reg(wm8978_dev_t *dev,uint8_t reg);
void wm8978_adda_cfg(wm8978_dev_t *dev,uint8_t dacen,uint8_t adcen);
void wm8978_input_cfg(wm8978_dev_t *dev,uint8_t micen,uint8_t lineinen,uint8_t auxen);
void wm8978_output_cfg(wm8978_dev_t *dev,uint8_t dacen,uint8_t bpsen);
void wm8978_mic_gain(wm8978_dev_t *dev,uint8_t gain);
void wm8978_linein_gain(wm8978_dev_t *dev,uint8_t gain);
void wm8978_aux_gain(wm8978_dev_t *dev,uint8_t gain);
void wm8978_hp_vol_set(wm8978_dev_t *dev,uint8_t voll,uint8_t volr);
void wm8978_spk_vol_set(wm8978_dev_t *dev,uint8_t volx);
void wm8978_i2s_cfg(wm8978_dev_t *dev,uint8_t fmt,uint8_t len);
void wm8978_three_d_set(wm8978_dev_t *dev,uint8_t depth);
void wm8978_eq_3d_dir(wm8978_dev_t *dev,uint8_t dir);
void wm8978_eq1_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain);
void wm8978_eq2_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain);
void wm8978_eq3_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain);
void wm8978_eq4_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain);
void wm8978_eq5_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain);
void wm8978_noise_set(wm8978_dev_t *dev,uint8_t enable,uint8_t gain);
void wm8978_alc_set(wm8978_dev_t *dev,uint8_t enable, uint8_t maxgain, uint8_t mingain);
void wm8978_sleep(wm8978_dev_t *dev,uint8_t enable);
void wm8978_resume(wm8978_dev_t *dev);
void wm8978_mono_out(wm8978_dev_t *dev,uint8_t enable);
void wm8978_spk_boost(wm8978_dev_t *dev,uint8_t enable);
void wm8978_sample_rate(wm8978_dev_t *dev,uint8_t srate);
void wm8978_loopback(wm8978_dev_t *dev,uint8_t enable);
void wm8978_a_mute(wm8978_dev_t *dev,uint8_t enable);

#endif
