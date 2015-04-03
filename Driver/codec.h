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

 

/****************************************/
extern unsigned char Set_Codec(unsigned char codec_control_type, unsigned char size_para, unsigned char *pdata);
extern unsigned char Get_Codec(unsigned char codec_control_type, unsigned char reg, unsigned char *pdata);

extern unsigned char Codec_Mixer(unsigned char i2c_channel );
//extern unsigned char Init_CODEC( unsigned int sample_rate );
extern unsigned char Init_CODEC( unsigned int sr, unsigned char sample_length ) ;

extern unsigned char Set_AIC3204_DSP_Offset( unsigned char slot_index ) ;
extern unsigned char Init_CODEC_AIC3204( unsigned int sample_rate ) ;

extern unsigned char CODEC_LOUT_Small_Gain_En( bool small_gain );
extern unsigned char CODEC_Set_Volume( unsigned int vol_spk,  unsigned int vol_lin );
extern unsigned char Check_SR_Support( unsigned int sample_rate );

#endif
