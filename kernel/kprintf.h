// kprintf.h

#ifndef __KPRINTF_H__
#define __KPRINTF_H__

void init_kprintf(void* putp, void (*putf) (void*, char));

void kprintf(char *fmt, ...);
void ksprintf(char* s,char *fmt, ...);

void kformat(void* putp, void (*putf) (void*, char), char *fmt, va_list va);


#endif // __KPRINTF_H__

