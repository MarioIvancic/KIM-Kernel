// kernel_arch.h
// arch specific include file
// arch = arm7tdmi

#ifndef __KERNEL_ARCH_H__
#define __KERNEL_ARCH_H__

#include <stdint.h>

// atomic integer type
typedef int atomic_t;
typedef unsigned int uatomic_t;

extern volatile atomic_t kernel_do_cs;


#endif // __KERNEL_ARCH_H__
