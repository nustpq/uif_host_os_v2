#ifndef __POWER_H__
#define __POWER_H__

#define POWER_SUPPLY

#define PDC_LEN  20

typedef struct __POWERADJ
{
    unsigned char Ch ;
    unsigned int Voltage ;
}POWERADJ ;

typedef struct __POWERCTR 
{
    unsigned char define ;
    unsigned char Ctr ;
}POWERCTR ;

extern void          ISR_POWER_UART(void);

extern CPU_INT08U CTR_PowerAdj( void *pdata );
extern CPU_INT08U CTR_PowerCtr( void *pdata );



extern CPU_INT08U combuf[PDC_LEN];



#endif

