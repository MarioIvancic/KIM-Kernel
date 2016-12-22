// lpc_timer_time.h
// use lpc21xx, lpc23xx, lpc24xx or lpc17xx 32-bit timer or pwm as system timer

#ifndef __LPC_TIMER_TIME_H__
#define __LPC_TIMER_TIME_H__

#include <stdint.h>

// inicijalizacija sistemskog tajmera i setovanje preskalera
// disable match registers, disable compare registers, disable external match register
void lpc_timer_time_init(void* timer_base_addr, int pclk_div, void* wdogfcn);

// vraca trenutni broj sistemskih tikova
uint32_t lpc_timer_time_get(void);

// vraca protekli broj sistemskih tikova od nekog trenutka
uint32_t lpc_timer_time_get_elapsed(uint32_t start_time);

// vrti se u petlji dok ne istekne zadati broj tikova i poziva wdog funkciju
void lpc_timer_time_pause(uint32_t duration);

#ifndef __LIBMCU__

#ifndef LPC_TIMER_CLK
    #define LPC_TIMER_CLK (PCLK)
#endif // LPC_TIMER_CLK

#define SEC2TIM(s)  (uint32_t)(( 1.0  * (s) * (LPC_TIMER_CLK)) + 0.5)
#define MSEC2TIM(m) (uint32_t)(( 1e-3 * (m) * (LPC_TIMER_CLK)) + 0.5)
#define USEC2TIM(u) (uint32_t)(( 1e-6 * (u) * (LPC_TIMER_CLK)) + 0.5)

#endif // __LIBMCU__

#endif // __LPC_TIMER_TIME_H__
