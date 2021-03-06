#ifndef __CODEC_H__
#define __CODEC_H__


#define PCA9546A_ADDRESS  0xE0
#define PCA9540B_ADDRESS  0xE0
#define PCA9548A_ADDRESS  0xEE

#define CODEC_ADDRESS     0x30 
#define AD1938_ADDRESS    0x04
#define AD1937_ADDRESS    0x08


/****************************************/
#define   ALC5610      10
#define   ALC5620      20
#define   PLL_Control0 0x0
#define   PLL_Control1 0x1
#define   DAC_Control0 0x2
#define   DAC_Control1 0x3
#define   DAC_Control2 0x4
#define   DAC_Mute     0x5
#define   DAC1L_Volume 0x6
#define   DAC1R_Volume 0x7
#define   DAC2L_Volume 0x8
#define   DAC2R_Volume 0x9
#define   DAC3L_Volume 0xa
#define   DAC3R_Volume 0xb
#define   DAC4L_Volume 0xc
#define   DAC4R_Volume 0xd
#define   ADC_Control0 0xE
#define   ADC_Control1 0xF
#define   ADC_Control2 0x10
#define   I2S_MODE     1
#define   TDM_MODE     2
#define   TDM16_MODE   3

 
typedef struct {
    unsigned short sr; // 8000 ~ 48000   
    unsigned char  sample_len ; //16 or 32 only 
    unsigned char  format; // 0 : i2s, 1 : pcm
    unsigned char  slot_num ; //2, 4, 8
    unsigned char  m_s_sel; //0 : master, 1 : slave;
    unsigned char  flag;  // flag if received audio_cfg command
    unsigned char  bclk_polarity; //1: frame start rising edge match bclk,  0: frame start rising edge inverted bclk
    unsigned char  delay;
    unsigned char  reserved[3];
}CODEC_SETS ;

/****************************************/
unsigned char Set_Codec(unsigned char codec_control_type, unsigned char size_para, unsigned char *pdata);
unsigned char Get_Codec(unsigned char codec_control_type, unsigned char reg, unsigned char *pdata);

unsigned char Codec_Mixer(unsigned char i2c_channel );
//unsigned char Init_CODEC( unsigned int sample_rate );

//unsigned char Init_CODEC( unsigned int sr, unsigned char sample_length, unsigned char format ,unsigned char slot_num,unsigned char master_or_slave) ;
unsigned char Init_CODEC( CODEC_SETS codec_set ) ;

unsigned char Set_AIC3204_DSP_Offset( unsigned char slot_index ) ;
unsigned char Init_CODEC_AIC3204( unsigned int sample_rate ) ;

unsigned char CODEC_LOUT_Small_Gain_En( bool small_gain );
unsigned char CODEC_Set_Volume( float vol_spk, float vol_lout, float vol_lin );
unsigned char Check_SR_Support( unsigned int sample_rate );

#endif
