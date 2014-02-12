//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Copyright (c) 2010-11 Code Red Technologies Ltd.
//
// LED flashing SysTick application for LPCXPresso1768 board
// This version of SysTick example changes the period that the
// LED is flashed at, starting off slowly, getting quicker, then
// slowing down again - then repeating.
//
// Software License Agreement
//
// The software is owned by Code Red Technologies and/or its suppliers, and is
// protected under applicable copyright laws.  All rights are reserved.  Any
// use in violation of the foregoing restrictions may subject the user to criminal
// sanctions under applicable laws, as well as to civil liability for the breach
// of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD.
//
//*****************************************************************************
#include "LPC17xx.h"                        /* LPC17xx definitions */

#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#include "leds.h"		// Header for LPCXPresso1768 LED2 routines

// Time period to wait between toggling LED. 1000 msecs = 1 secs.
#define DELAY_LEN_MAX 250
#define DELAY_LEN_MIN 20
#define DELAY_CHANGE 5

uint32_t flashval; // current length of led flash

// The following variables are used to store the maximum and minimum led flash
// periods, and the amount the period changes by each iteration.
// These do no strictly need to be variables, but having them as globals means
// that the behaviour of the application code be easily changed from the debugger
// when the code is being run.
uint32_t maxflashval, minflashval, changeval;

// If set to none-zero, then speed of flash will stay constant
uint32_t constflash = 0;

// Controls whether flash period change is a decrease or increase
int32_t sign = +1 ;


volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
__INLINE static void systick_delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/* Main Program */

int main (void) {

	// Set up initial flash period, max and min flash periods, and change value
	flashval = DELAY_LEN_MAX;
	maxflashval = DELAY_LEN_MAX;
	minflashval = DELAY_LEN_MIN;
	changeval = DELAY_CHANGE;

	led2_init();	// Setup GPIO for LED2

	// Setup SysTick Timer for 1 msec interrupts
	if (SysTick_Config(SystemCoreClock / 1000))
	{ while (1); // Capture error
	}

  while (1)                                /* Loop forever */
  {
	  // Turn LED on, then wait
	  led2_on();
	  systick_delay (flashval);

	  // Turn LED off, then wait
	  led2_off();
	  systick_delay (flashval);

	  // Check to see if changing flash period is required
	  if (constflash  == 0)
	  {
		  // Check to see if the flash period is at min or max point
		  if (flashval <= minflashval)
		  {	  // Reverse direction of change
			  sign = +1;
			  flashval = minflashval;
		  }
		  else if (flashval >= maxflashval)
		  {
			  sign = -1;
			  flashval = maxflashval;
		  }

		  // Modify the flash period (either increase or decrease
		  // depending on sign)
		  flashval = flashval + (sign * changeval);
	  }
	}


  return 0;	// Should never reach here
}
