// sys_init.h

#ifndef __SYS_INIT_H__
#define __SYS_INIT_H__


// power-down unused devices
// starts up the PLL
// sets up GPIO pins
// sets up ADC & DAC pins
// wait for PLL to lock.
// connect to PLL clock
// setup & enable the MAM
// set interrup vectors mapping
void sys_init(void);

#endif // __SYS_INIT_H__
