//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Copyright (c) 2011 Code Red Technologies Ltd.
//
// timer_main.c sets up the 4 timers to be found in the LPC1768/9 MCU fitted
// to the LPCXpresso176x board, and toggles the LED based on when the
// timers trigger interrupts.
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

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#include "leds.h"
#include "timer.h"

#define TIMER0_INTERVAL	((2 * (4 *(SystemCoreClock/10))) - 1)
#define TIMER1_INTERVAL	((2 *(4 *(SystemCoreClock/10))/3) - 1)
#define TIMER2_INTERVAL	((2 *(4 *(SystemCoreClock/10))/5) - 1)
#define TIMER3_INTERVAL	((2 *(4 *(SystemCoreClock/10))/7) - 1)

#define PCTIM0_POWERON (1 << 1)
#define PCTIM1_POWERON (1 << 2)
#define PCTIM2_POWERON (1 << 22)
#define PCTIM3_POWERON (1 << 23)

int main(void) {
	
	led2_init(); // Setup GPIO for led2 (on GPIO0.22)

	// Turn on power to TIMER2 and TIMER3
	// (TIMER0 and TIMER1 on by default)
	LPC_SC->PCONP |=  PCTIM2_POWERON | PCTIM3_POWERON;

	// Initialise all timers
	init_timer( 0, TIMER0_INTERVAL );
	init_timer( 1, TIMER1_INTERVAL );
	init_timer( 2, TIMER2_INTERVAL );
	init_timer( 3, TIMER3_INTERVAL );

	// Enable all timers
	enable_timer( 0 );
	enable_timer( 1 );
	enable_timer( 2 );
	enable_timer( 3 );

	// Enter an infinite loop, just incrementing a counter
	volatile static int i = 0 ;
	while(1) {
		i++ ;
	}
	return 0 ;
}
