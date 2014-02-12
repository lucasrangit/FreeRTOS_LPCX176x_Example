Code Red Technologies LPCXpresso176x - systick_twinkle demo
===========================================================

This project contains a LED flashing Systick example for the 
LPCXpresso board mounted with an LPC1768/9 Cortex-M3 part. The
period of the LED flash will continously change.

When executed, the code will configure the GPIO so that LED2
can be controlled, and set up the Systick timer (built into 
the Cortex-M3 CPU) so that a timer tick takes place every 
1/1000th of a second.

The code will then flash LED2 on and off. The period of this
flash will start off relatively slowly, get faster, and then 
slow down.

If you stop the code whilst it is running, you can modify how
fast and slow the flashing gets, and how quickly it changes by
modifying the variables 'maxflashval', 'minflashval' and 
'changeval'.

The project makes use of code from the following library project:
- CMSISv2p00_LPC17xx : for CMSIS 2.00 files relevant to LPC17xx

This library project must exist in the same workspace in order
for the project to successfully build.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Note that this example project is intended for use with Red Suite /
LPCXpresso IDE v4.0 or later.

Use of this project with earlier versions of the Code Red IDE is 
not recommended or supported.
