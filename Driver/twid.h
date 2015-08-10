/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef TWID_H
#define TWID_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <bsp.h>
#include "async.h"

//------------------------------------------------------------------------------
//         Global definitions
//------------------------------------------------------------------------------

/// TWI driver is currently busy.
#define TWID_NO_ERROR                0
#define TWID_ERROR_BUSY              1
#define TWID_ERROR_TIMEOUT           2
#define TWID_ERROR_TIMEOUT2          3
/// TWI clock frequency in Hz.
#define TWCK            200000


//------------------------------------------------------------------------------
//         Global types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// TWI driver structure. Holds the internal state of the driver.
//------------------------------------------------------------------------------
typedef struct _Twid {

    /// Pointer to the underlying TWI peripheral.
    AT91S_TWI *pTwi;
    /// Current asynchronous transfer being processed.
    Async *pTransfer;

} Twid;

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

extern void TWID_Initialize(Twid *pTwid, AT91S_TWI *pTwi);


extern unsigned char TWID_Read(
    unsigned char address,
    unsigned int iaddress,
    unsigned char isize,
    unsigned char *pData,
    unsigned int num,
    Async *pAsync);

extern unsigned char TWID_Write(
    unsigned char address,
    unsigned int iaddress,
    unsigned char isize,
    unsigned char *pData,
    unsigned int num,
    Async *pAsync);

extern  void TWI_Init( unsigned int twi_clock );

extern   Twid twid;


extern OS_EVENT *TWI_Sem_lock; //sem for TWI
extern OS_EVENT *TWI_Sem_done; //sem for TWI

extern void TWI_Init_Re( void );

#endif //#ifndef TWID_H

