#ifndef __FREQ_H
#define __FREQ_H

#define FREQUENCE_SUPPLY


typedef struct __FRQADJ
{
    unsigned char   Ch ;
    unsigned char   FrqType ;
    unsigned int    Frq ;
    unsigned short  Volt;
}FRQADJ ;

typedef struct __FRQCTR
{
    unsigned char Ch ;
    unsigned char Ctr ;
}FRQCTR ;


extern  CPU_INT08U CTR_FrqAdj( void *pdata );
extern  CPU_INT08U CTR_FrqCtr( void *pdata );
 

#endif

