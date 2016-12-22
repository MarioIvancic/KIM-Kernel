// crt0.h
// deklaracije funkcija i varijabli iz crt0.S

#ifndef _CRT0_H_
#define _CRT0_H_

// declare functions and values from crt0.S & the linker control file
extern char str_serial_number[];

// adresa odakle pocinje upisivi dio flash-a, definise se u crt0.S i linker-skriptu
extern int user_flash_start[];

// kraj staticki alociranih varijabli - pocetak heapa
extern int __heap_start[];

// code read protection word: 0x87654321 -- ON, 0x00000000 -- OFF
extern unsigned int code_read_protection_word;


// disable interrupts and enter endless loop
extern void __exit(void);

//#define reset _exit
//#define abort _exit
//#define _abort _exit
//#define _reset _exit

// maybe add interrupt vector addresses

// funcija koja je u stvari na adresi reset vektora
extern void _boot(void);

#endif // _CRT0_H_
