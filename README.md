# KIM-Kernel
Real time preemptive kernel for ARM Cortex M3 and ARM7-TDMI

This is small preemptive kernel for ARM Cortex-m3 and ARM7-TDMI featuring
-	mutex
-	semaphore
-	queue
-	spinlock
-	spinsem (spinlock based semaphore)
-	kprintf (spinlock protected printf)
-	time quota and priority based task scheduler

Original idea came from the article (actually from some older version of this
article which I can't find now):

http://coactionos.com/embedded%20design%20tips/2013/10/09/Tips-Context-Switching-on-the-Cortex-M3/

After that I reviewed a number of RTOSes and came up with something of my own 
(with a few things borrowed from others).

Mutex, semaphore and queue API are modeled on posix API.

I tested this kernel code on lpcxpresso board with LPC1769 but I never used it 
for any real project. For flashing I used lpc21isp and for debugging just my 
brain, printf and LEDs. The code is originaly developed on windows using 
CodeBlocks IDE and WinARM toolchain but before this release I switched to Linux
Mint. Changes are in the first few lines of Makefile (BUILD_TIMESTAMP_ and 
TCHAIN_PREFIX).

I want to thank the following people:
CoActionOS for mentioned article that started all this,
Jakub Jermar (HelenOS) for his doubly linked circular list implementation,
Kustaa Nyholm / SpareTimeLabs for printf, sprintf and tfp_printf,
Martin THOMAS for his WinARM toolchain and great ARM projects.

Mario Ivančić

