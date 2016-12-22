// kprintf.c
// spinlock protected printf implementation
// based on Kustaa Nyholm / SpareTimeLabs tfp_printf/tfp_format

#include "printf.h"
#include "kprintf.h"
#include "kspinlock.h"

typedef void (*putcf) (void*, char);
static putcf stdout_putf;
static void* stdout_putp;

static int lock;

void init_kprintf(void* putp,void (*putf) (void*,char))
{
	stdout_putf=putf;
	stdout_putp=putp;
	lock = KSPINLOCK_UNLOCKED;
}


void kprintf(char *fmt, ...)
{
	va_list va;
	va_start(va,fmt);
	kformat(stdout_putp,stdout_putf,fmt,va);
	va_end(va);
}


static void putcp(void* p,char c)
{
	*(*((char**)p))++ = c;
}


void ksprintf(char* s,char *fmt, ...)
{
	va_list va;
	va_start(va,fmt);
	tfp_format(&s,putcp,fmt,va);
	putcp(&s,0);
	va_end(va);
}


void kformat(void* putp, void (*putf) (void*, char), char *fmt, va_list va)
{
	kspinlock_lock(&lock);
	tfp_format(putp, putf, fmt, va);
	kspinlock_unlock(&lock);
}


