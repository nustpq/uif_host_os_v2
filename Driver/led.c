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

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "led.h"
//#include <pio.h>
#include <bsp.h>
//------------------------------------------------------------------------------
//         Local Variables
//------------------------------------------------------------------------------

#ifdef PINS_LEDS
static const Pin pinsLeds[]         = {PINS_LEDS};
static const unsigned int numLeds   = PIO_LISTSIZE(pinsLeds);
#endif

static const Pin pinsBuzzer[]         = {BUZZER};


//------------------------------------------------------------------------------
//         Global Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Configures the pin associated with the given LED number. If the LED does
/// not exist on the board, the function does nothing.
/// \param led  Number of the LED to configure.
/// \return 1 if the LED exists and has been configured; otherwise 0.
//------------------------------------------------------------------------------
unsigned char LED_Configure(unsigned int led)
{
#ifdef PINS_LEDS
    // Check that LED exists
    if (led >= numLeds) {

        return 0;
    }

    // Configure LED
    return (PIO_Configure(&pinsLeds[led], 1));
#else
    return 0;
#endif
}

//------------------------------------------------------------------------------
/// Turns the given LED on if it exists; otherwise does nothing.
/// \param led  Number of the LED to turn on.
/// \return 1 if the LED has been turned on; 0 otherwise.
//------------------------------------------------------------------------------
unsigned char LED_Set(unsigned int led)
{
#ifdef PINS_LEDS
    // Check if LED exists
    if (led >= numLeds) {

        return 0;
    }

    // Turn LED on
    if (pinsLeds[led].type == PIO_OUTPUT_0) {

        PIO_Set(&pinsLeds[led]);
    }
    else {

        PIO_Clear(&pinsLeds[led]);
    }

    return 1;
#else
    return 0;
#endif
}

//------------------------------------------------------------------------------
/// Turns a LED off.
/// \param led  Number of the LED to turn off.
/// \param 1 if the LED has been turned off; 0 otherwise.
//------------------------------------------------------------------------------
unsigned char LED_Clear(unsigned int led)
{
#ifdef PINS_LEDS
    // Check if LED exists
    if (led >= numLeds) {

        return 0;
    }

    // Turn LED off
    if (pinsLeds[led].type == PIO_OUTPUT_0) {

        PIO_Clear(&pinsLeds[led]);
    }
    else {

        PIO_Set(&pinsLeds[led]);
    }

    return 1;
#else
    return 0;
#endif
}

//------------------------------------------------------------------------------
/// Toggles the current state of a LED.
/// \param led  Number of the LED to toggle.
/// \return 1 if the LED has been toggled; otherwise 0.
//------------------------------------------------------------------------------
unsigned char LED_Toggle(unsigned int led)
{
#ifdef PINS_LEDS
    // Check if LED exists
    if (led >= numLeds) {

        return 0;
    }

    // Toggle LED
    if (PIO_GetOutputDataStatus(&pinsLeds[led])) {

        PIO_Clear(&pinsLeds[led]);
    }
    else {

        PIO_Set(&pinsLeds[led]);
    }

    return 1;
#else
    return 0;
#endif
}



//------------------------------------------------------------------------------
///        
//------------------------------------------------------------------------------
void ChannelSet(unsigned char hs,unsigned char sel)
{ /*
  	if(hs)
	{
	  	PIO_Clear(&pinsLeds[3]);
	}
	else
	{
	  	PIO_Set(&pinsLeds[3]);
	}
	if(sel < 4)
	{
	 	PIO_Set(&pinsLeds[0]); 
		PIO_Set(&pinsLeds[1]);  
		PIO_Set(&pinsLeds[2]);
		PIO_Set(&pinsLeds[4]);
		if(sel > 2) 
		{
			PIO_Clear(&pinsLeds[sel+1]);
		}
		else
		{
		  	PIO_Clear(&pinsLeds[sel]);
		}
	}
  */
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int IOInit(void)
{  /*
  	unsigned int x;	
	for(x = 0;x < numLeds;x++)
	{
	  	LED_Configure(x);
	}
	//PIO_Configure(pinsKeys, PIO_LISTSIZE(pinsKeys)); 
	
   */
  	return 0;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int IOGetKey(int x)
{
   /*
  	unsigned int tt;
	static int lastKey = -1,keyCnt = 0;
	
  	tt = (AT91C_BASE_PIOB->PIO_PDSR& (0x1F << 3)) >> 3;
	tt = ~tt;
	if(keyCnt++ > x)
	{
	  	keyCnt = 0;
		if(tt &0x1)//KEY 0
		{
		  	return 0;
		}
		else if(tt &0x2)//KEY 1
		{
		  	return 1;
		}
		else if(tt &0x4)//KEY 3
		{
		  	return 3;
		}
		else if(tt &0x8)//KEY 2
		{
		  	return 2;
		}
		else if(tt &0x10)//KEY 4
		{
		  	return 4;
		}
	}
  */
	return -1;
}



//------------------------------------------------------------------------------
///
///
///
//------------------------------------------------------------------------------
volatile unsigned char BUZZER_MUTE = 0 ;

void Buzzer_OnOff( unsigned char onoff )
{
  
    if( BUZZER_MUTE != 0 ) {
        return;
    }
    
    if( onoff == 1 ) {
        PIO_Clear( &pinsBuzzer[0] ); //buzzer 
        
    } else {
        PIO_Set( &pinsBuzzer[0] ); //buzzer 
        
    }
    
    
}

void Buzzer_Toggle( void )
{
   
    if( BUZZER_MUTE != 0 ) {
        return;
    }
    // Toggle Buzz
    if (PIO_GetOutputDataStatus(&pinsBuzzer[0])) {

        PIO_Clear(&pinsBuzzer[0]);
    }
    else {

        PIO_Set(&pinsBuzzer[0]);
    }
    
    
}

