// arm7tdmi_exception_handler.c
// data abort handler and program abort handler

/*
	http://www.embedded.com/design/prototyping-and-development/4006695/How-to-use-ARM-s-data-abort-exception
	
	On LPC2148 data abort can be triggered by:

    - performing a write to ROM (flash) space
    - accessing any of the Reserved Areas defined in the LPC 2148 User Manual
    Note: On LPC2148 unaligned memory access will not trigger data abort but on Atmel Sam7S will.
*/

// pointer to print char function
static int (*abt_putchar)(int);
typedef int (*int_int_fcn)(int);

void set_eh_output(void* fcn);

void set_eh_output(void* fcn)
{
    abt_putchar = (int_int_fcn) fcn;
}

// 19 * 4 = 76 bytes
// stack frame: PC, OPCODE, CPSR, LR, SP, FLAG, R0, .., R12
typedef struct
{
	unsigned pc; 	// r15
	unsigned inst;
	unsigned cpsr; 	// originalni cpsr
	unsigned lr; 	// r14
	unsigned sp; 	// r13
	unsigned abt_type;	// 0: program ABT, 1: data ABT, 2: undefined ABT
	unsigned r0;
	unsigned r1;
	unsigned r2;
	unsigned r3;
	unsigned r4;
	unsigned r5;
	unsigned r6;
	unsigned r7; 	// fp for THUMB
	unsigned r8;
	unsigned r9;
	unsigned r10; 	// sl
	unsigned r11; 	// fp for ARM
	unsigned r12; 	// ip, scratch register
} abort_context;



static void abt_puts(const char* s)
{
    if(!abt_putchar) return;
	while(*s)
	{
		abt_putchar(*s);
		s++;
	}
}

// xxxx000xxxxxxxxxxxxxxxxx1xx1xxxx Extra Load/Store A3-5
// xxxx000PUIWLnnnnddddmmmm1SH1mmmm Load/Store half and double word, byte sign extend, A5-33
// xxxx010PUBWLnnnnddddiiiiiiiiiiii Load/Store Immediate offset, A5-18
// xxxx011PUBWLnnnnddddaaaaass0mmmm Load/Store reg offset
// xxxx100PUSWLnnnnllllllllllllllll Load/Store multiple, A5-41

static void abt_hex(unsigned long x)
{
	static const char hc[] = {"0123456789ABCDEF"};
	char hb[12];
	int i;
	hb[0] = '0';
	hb[1] = 'x';
	for(i = 9; i > 1; i--, x >>= 4)
	{
		hb[i] = hc[x & 0x0f];
	}
	hb[10] = 0;
	abt_puts(hb);
}


void data_program_abort_handler(abort_context *ctx)
{
	unsigned i, *p;
	
	abt_puts("\r\n");
	if(ctx->abt_type == 0) abt_puts("PROGRAM");
	else if(ctx->abt_type == 1) abt_puts("DATA");
	else abt_puts("UNDEFINED INSTRUCTION");
	abt_puts(" ABORT\r\npc="); abt_hex(ctx->pc);
	if(ctx->abt_type == 1) { abt_puts("  [pc]="); abt_hex(ctx->inst); } abt_puts("\r\n");
	
	abt_puts("Dumping Registers [r10=sl; r11=fp; r12=ip; r13=sp; r14=lr; r15=pc; r7=fp(Thumb)]\r\n");
	
	abt_puts("r0 ="); abt_hex(ctx->r0); 
	abt_puts("  r1 ="); abt_hex(ctx->r1);
	abt_puts("  r2 ="); abt_hex(ctx->r2); 
	abt_puts("  r3 ="); abt_hex(ctx->r3); abt_puts("\r\n");
	
	abt_puts("r4 ="); abt_hex(ctx->r4); 
	abt_puts("  r5 ="); abt_hex(ctx->r5);
	abt_puts("  r6 ="); abt_hex(ctx->r6); 
	abt_puts("  r7 ="); abt_hex(ctx->r7); abt_puts("\r\n");
	
	abt_puts("r8 ="); abt_hex(ctx->r8); 
	abt_puts("  r9 ="); abt_hex(ctx->r9);
	abt_puts("  r10="); abt_hex(ctx->r10); 
	abt_puts("  r11="); abt_hex(ctx->r11); abt_puts("\r\n");
	
	abt_puts("r12="); abt_hex(ctx->r12); 
	abt_puts("  sp ="); abt_hex(ctx->sp); 
	abt_puts("  lr ="); abt_hex(ctx->lr); 
	abt_puts(" cpsr="); abt_hex(ctx->cpsr); abt_puts("\r\n");
	abt_puts("Legend: r0  r1  r2  r3  r4  r5  r6  r7  r8  r9  r10 r11 r12 r13 r14 r15\r\n");
	abt_puts("                                            sb  sl  fp  ip  sp  lr  pc\r\n");
	abt_puts("        a1  a2  a3  a4  v1  v2  v3  v4  v5  v6  v7  v8\r\n");
	
	abt_puts("\r\nDumping stack:\r\n");
	for(p = (unsigned*) ctx->sp, i = 0; i < 32; i++, p++)
	{
		if((i & 3) == 0)
		{
			abt_hex((int)p);
			abt_puts(":");
		}
		abt_puts(" ");
		abt_hex(*p);
		if((i & 3) == 3) abt_puts("\r\n");
	}

	abt_puts("\r\n");
	abt_puts("Program halted\r\n");
}
