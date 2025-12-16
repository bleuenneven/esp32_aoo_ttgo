/*******************************************************************************
 I2C Configuration for a Wolfson WM8978 Audio Codec.
 This file was written based on the information provided in the Codec Specs
 and on the NuovotonDuino project: https://github.com/DFRobot/NuvotonDuino.
 *******************************************************************************/
 
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "WM8978.h"
#include "driver/i2c.h"


static uint16_t WM8978_REGVAL_TBL[58]=
{
    0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
    0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
    0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
    0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
    0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
    0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
    0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
    0X0001,0X0001
};

// WM8978 init
uint8_t wm8978_init(wm8978_dev_t *dev)
{
    wm8978_init_i2c();
    uint8_t res;
    res = wm8978_write_reg(dev,0,0);    // WM8978 reset
    if(res) return 1;
    wm8978_write_reg(dev,1,0X9B);       // R1, OUT4MIXEN, MICEN (MIC), BIASEN, VMIDSEL[1:0] 
    wm8978_write_reg(dev,2,0X1B0);      // R2, ROUT1, LOUT1, BOOSTENR, BOOSTENL 
    wm8978_write_reg(dev,3,0X16C);      // R3, OUT4EN, LOUT2EN, ROUT2EN, RMIXEN, LMIXEN 
    wm8978_write_reg(dev,6,0);          // R6, MCLK 
    wm8978_write_reg(dev,43,1<<4);      // R43, INVROUT2
    wm8978_write_reg(dev,47,1<<8);      // R47, PGABOOSTL, MIC
    wm8978_write_reg(dev,48,1<<8);      // R48, PGABOOSTR, MIC
    wm8978_write_reg(dev,49,1<<1);      // R49, TSDEN
    wm8978_write_reg(dev,10,1<<3);      // R10, DACOSR
    wm8978_write_reg(dev,14,1<<3);      // R14, ADCOSR
    return 0;
}

// Initialize i2c for the ESP32
void wm8978_init_i2c(void)
{
    i2c_port_t i2c_master_port = (i2c_port_t) I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t) I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t) I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0; 
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_DISABLE,
                       I2C_MASTER_TX_BUF_DISABLE, 0);
}

// WM8978 register write
// reg: Register Address
// val: Register value
uint8_t wm8978_write_reg(wm8978_dev_t *dev,uint8_t reg,uint16_t val)
{
    unsigned char buf[2];
    buf[0] = (reg<<1)|((val>>8)&0X01);
    buf[1] = val&0XFF;
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (WM8978_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, buf, 2, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin((i2c_port_t) 1, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    
    dev->regval_tbl[reg] = val;
    return 0;
}

// WM8978 read register
// Reads the value of the local register buffer zone
// reg: Register Address
// Return Value: Register value
uint16_t wm8978_read_reg(wm8978_dev_t *dev,uint8_t reg)
{
    return dev->regval_tbl[reg];
}

// WM8978 DAC/ADC
// adcen: adc 1/0
// dacen: dac 1/0
void wm8978_adda_cfg(wm8978_dev_t *dev,uint8_t dacen,uint8_t adcen)
{
    uint16_t regval;
    regval = wm8978_read_reg(dev,3);        // R3
    if(dacen) regval |= 3<<0;               // R3 DACR & DACL        
    else regval &= ~(3<<0);                 // R3 DACR & DACL 
    wm8978_write_reg(dev,3,regval);         // R3
    
    regval=wm8978_read_reg(dev,2);          // R2
    if(adcen) regval|=3<<0;                 // R2 ADCR & ADCL
    else regval&=~(3<<0);                   // R2 ADCR & ADCL
    wm8978_write_reg(dev,2,regval);         // R2
}

// WM8978 Input Config
// micen: MIC 1/0
// lineinen: Line In 1/0
// auxen: aux 1/0
void wm8978_input_cfg(wm8978_dev_t *dev,uint8_t micen,uint8_t lineinen,uint8_t auxen)
{
    uint16_t regval;
    regval = wm8978_read_reg(dev,2);        // R2
    if(micen) regval|=3<<2;                 // INPPGAENR, INPPGAENL (MIC/PGA)
    else regval&=~(3<<2);
    wm8978_write_reg(dev,2,regval);         // R2
    
    regval=wm8978_read_reg(dev,44);         // R44
    if(micen) regval|=3<<4|3<<0;            // LIN2INPPGA, LIP2INPGA, RIN2INPPGA, RIP2INPGA
    else regval&=~(3<<4|3<<0);
    wm8978_write_reg(dev,44,regval);        // R44
    
    if(lineinen) wm8978_linein_gain(dev,5); // LINE IN 0dB
    else wm8978_linein_gain(dev,0);
    if(auxen) wm8978_aux_gain(dev,7);       // AUX 6dB
    else wm8978_aux_gain(dev,0);
}

// WM8978
// dacen: DAC 1/0
// bpsen: Bypass (MIC, LINE IN,AUX) 1/0
void wm8978_output_cfg(wm8978_dev_t *dev,uint8_t dacen,uint8_t bpsen)
{
    uint16_t regval = 0;
    if(dacen) regval|=1<<0;             // DAC
    if(bpsen)
    {
        regval|=1<<1;                   // BYPASS
        regval|=5<<2;                   // 0dB
    }
    wm8978_write_reg(dev,50,regval);    // R50
    wm8978_write_reg(dev,51,regval);    // RR1
}

// WM8978 MIC (BOOST 20dB,MIC-->ADC)
// gain: 0~63, -12dB~35.25dB, 0.75dB/Step
void wm8978_mic_gain(wm8978_dev_t *dev,uint8_t gain)
{
    gain &= 0X3F;
    wm8978_write_reg(dev,45,gain);          // R45, PGA
    wm8978_write_reg(dev,46,gain|1<<8);     // R46, PGA
}

// WM8978 L2/R2 (Line In) (L2/R2-->ADC)
// gain: 0~7, 0ֹ, 1~7, -12dB~6dB, 3dB/Step
void wm8978_linein_gain(wm8978_dev_t *dev,uint8_t gain)
{
    uint16_t regval;
    gain &= 0X07;
    regval = wm8978_read_reg(dev,47);           // R47
    regval&=~(7<<4);
    wm8978_write_reg(dev,47,regval|gain<<4);    // R47
    regval = wm8978_read_reg(dev,48);           // R48
    regval&=~(7<<4);
    wm8978_write_reg(dev,48,regval|gain<<4);    // R48
}

// WM8978 AUXR, AUXL(PWM) (AUXR/L-->ADC)
// gain:0~7, 0ֹ, 1~7, -12dB~6dB, 3dB/Step
void wm8978_aux_gain(wm8978_dev_t *dev,uint8_t gain)
{
    uint16_t regval;
    gain &= 0X07;
    regval = wm8978_read_reg(dev,47);           // R47
    regval&=~(7<<0);
    wm8978_write_reg(dev,47,regval|gain<<0);    // R47
    regval = wm8978_read_reg(dev,48);           // R48
    regval&=~(7<<0);
    wm8978_write_reg(dev,48,regval|gain<<0);    // R48
}

// I2S
// fmt: 0, LSB; 1,MSB; 2, I2S; 3, PCM/DSP;
// len:0,16λ;1,20λ;2,24λ;3,32λ;
void wm8978_i2s_cfg(wm8978_dev_t *dev,uint8_t fmt,uint8_t len)
{
    fmt &= 0X03;
    len &= 0X03;
    wm8978_write_reg(dev,4,(fmt<<3)|(len<<5));     // R4, WM8978
}

// Headphone Output Volume
// voll: (0~63)
// volr: (0~63)
void wm8978_hp_vol_set(wm8978_dev_t *dev,uint8_t voll,uint8_t volr)
{
    voll &= 0X3F;
    volr &= 0X3F;
    if(voll==0) voll|=1<<6;
    if(volr==0) volr|=1<<6;
    wm8978_write_reg(dev,52,voll);          // R52
    wm8978_write_reg(dev,53,volr|(1<<8));   // R53 (HPVU=1)
}

// Speaker Volume
// voll: (0~63)
void wm8978_spk_vol_set(wm8978_dev_t *dev,uint8_t volx)
{
    volx &= 0X3F;
    if(volx==0) volx|=1<<6;
    wm8978_write_reg(dev,54,volx);          // R54
    wm8978_write_reg(dev,55,volx|(1<<8));   // R55 (SPKVU=1)
}

// 3D Setup
// depth: 0~15
void wm8978_three_d_set(wm8978_dev_t *dev,uint8_t depth)
{
    depth &= 0XF;
    wm8978_write_reg(dev,41,depth);     // R41
}

// EQ/3D
// dir:0, ADC
// 1,DAC
void wm8978_eq_3d_dir(wm8978_dev_t *dev,uint8_t dir)
{
    uint16_t regval;
    regval = wm8978_read_reg(dev,0X12);
    if(dir) regval|=1<<8;
    else regval&=~(1<<8);
    wm8978_write_reg(dev,18,regval);     // R18
}

// EQ1
// cfreq: 0~3, 80/105/135/175Hz
// gain: 0~24, -12~+12dB
void wm8978_eq1_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain)
{
    uint16_t regval;
    cfreq &= 0X3;
    if(gain>24) gain = 24;
    gain = 24-gain;
    regval = wm8978_read_reg(dev,18);
    regval &= 0X100;
    regval |= cfreq<<5;
    regval |= gain;
    wm8978_write_reg(dev,18,regval);      // R18, EQ1
}

// EQ2
// cfreq: 0~3, 230/300/385/500Hz
// gain: 0~24, -12~+12dB
void wm8978_eq2_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3;
    if(gain>24) gain = 24;
    gain = 24-gain;
    regval |= cfreq<<5;
    regval |= gain;
    wm8978_write_reg(dev,19,regval);
}

// EQ3
// cfreq: 0~3, 650/850/1100/1400Hz
// gain: 0~24, -12~+12dB
void wm8978_eq3_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3;
    if(gain>24) gain = 24;
    gain = 24-gain;
    regval |= cfreq << 5;
    regval |= gain;
    wm8978_write_reg(dev,20,regval);     // R20, EQ3
}

// EQ4
// cfreq: 0~3, 1800/2400/3200/4100Hz
// gain: 0~24, -12~+12dB
void wm8978_eq4_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3;
    if(gain>24) gain = 24;
    gain = 24-gain;
    regval |= cfreq << 5;
    regval |= gain;
    wm8978_write_reg(dev,21,regval);    // R21, EQ4
}

// EQ5
// cfreq: 0~3, 5300/6900/9000/11700Hz
// gain: 0~24, -12~+12dB
void wm8978_eq5_set(wm8978_dev_t *dev,uint8_t cfreq,uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3;
    if(gain>24) gain = 24;
    gain = 24-gain;
    regval |= cfreq << 5;
    regval |= gain;
    wm8978_write_reg(dev,22,regval);    // R22, EQ5
}

void wm8978_alc_set(wm8978_dev_t *dev,uint8_t enable, uint8_t maxgain, uint8_t mingain)
{
    uint16_t regval;
    
    if(maxgain>7) maxgain=7;
    if(mingain>7) mingain=7;
    
    regval=wm8978_read_reg(dev,32);
    if(enable)
        regval |= (3<<7);
    regval |= (maxgain<<3)|(mingain<<0);
    wm8978_write_reg(dev,32,regval);
}

void wm8978_noise_set(wm8978_dev_t *dev,uint8_t enable,uint8_t gain)
{
    uint16_t regval;
    
    if(gain>7) gain=7;
    
    regval = wm8978_read_reg(dev,35);
    regval = (enable<<3);
    regval |= gain;
    wm8978_write_reg(dev,35,regval);    // R35
}


/**********************************************************
Additional functions written for PICO DSP development board
https://github.com/ohmic-net/pico_dsp 
**********************************************************/

// Sleep mode for power saving 
// 1 to enable device standby mode
void wm8978_sleep(wm8978_dev_t *dev,uint8_t enable)
{ 
    uint16_t regval;
    regval = (enable<<6);
    wm8978_write_reg(dev,2,regval);     // R2, sleep
}

// Resume / wake up from sleep 
void wm8978_resume(wm8978_dev_t *dev)
{ 
    uint16_t regval;
    regval = ~(1<<6);
    wm8978_write_reg(dev,2,regval);
}

// OUT4 (Mono Output) enable
void wm8978_mono_out(wm8978_dev_t *dev,uint8_t enable)
{ 
    uint16_t regval;
    regval = (enable<<3);
    wm8978_write_reg(dev,57,regval);    //LDAC2OUT4
}

// Speaker boost 
void wm8978_spk_boost(wm8978_dev_t *dev,uint8_t enable)
{
    uint16_t regval;
    regval = (enable<<2);
    wm8978_write_reg(dev,49,regval);
}

// Set Sample Rate
// srate: 0~5 , 48kHz, 32kHz, 24kHz, 16kHz, 12kHz, 8kHz
void wm8978_sample_rate(wm8978_dev_t *dev,uint8_t srate)
{
    uint16_t regval = 0;                // 48kHz, default
    if(srate==1) regval = 0x2;          
    if(srate==2) regval = 0x4;
    if(srate==3) regval = 0x6;
    if(srate==4) regval = 0x8;
    if(srate==5) regval = 0xA;
    wm8978_write_reg(dev,7,regval);     // R7, Additional Ctrl
}

// Loopback, ADC data output is fed directly into DAC data input
void wm8978_loopback(wm8978_dev_t *dev,uint8_t enable)
{
    uint16_t regval;
    regval = enable;
    wm8978_write_reg(dev,5,regval);
}

// DAC Automute (after 1024 consective zeros)
void wm8978_a_mute(wm8978_dev_t *dev,uint8_t enable)
{
    uint16_t regval;
    regval = enable<<2;
    wm8978_write_reg(dev,10,regval);
}

