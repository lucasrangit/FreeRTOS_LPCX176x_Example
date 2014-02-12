LPCXpresso176x_timers
=====================
This project contains a LED flashing timer example for the 
LPCXpresso board mounted with an LPC176x Cortex-M3 part.

When executed, the code will configure the GPIO so that LED2
can be controlled, and set up the 4 timers (built into 
the Cortex-M3 CPU) so that timer ticks from each of the timers
take place out of sequence with each other

The code will then flash LED2 on and off, as timer interrupts occur.

The project makes use of code from the following library project:
- CMSISv1p30_LPC17xx : for CMSIS 1.30 files relevant to LPC17xx

This library project must exist in the same workspace in order
for the project to successfully build.

~~~~~~~~~~~~
Note that this example is only suitable for use with Red Suite / 
LPCXpresso IDE v3.6.x (v3.8.x for Linux) or later, as it makes 
use of linker related functionality introduced in this release.

More details at:

http://support.code-red-tech.com/CodeRedWiki/EnhancedManagedLinkScripts


