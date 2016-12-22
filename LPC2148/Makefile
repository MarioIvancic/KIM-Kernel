#BUILD_TIMESTAMP_ := $(shell C:/WinAVR-20100110/utils/bin/date +'"%Y-%m-%d_%H:%M:%S"')
BUILD_TIMESTAMP_ := $(shell date +'"%Y-%m-%d_%H:%M:%S"')
#BUILD_TIMESTAMP := $(shell C:/WinAVR-20100110/utils/bin/date +'"%Y-%m-%d_%H:%M:%S"')
#BUILD_TIMESTAMP_ := $(shell C:/WinAVR-20100110/utils/bin/date +'%Y-%m-%d_%H:%M:%S')
#BUILD_TIMESTAMP := "$(BUILD_TIMESTAMP)"
BUILD_TIMESTAMP := $(subst \,",$(BUILD_TIMESTAMP_))

# Toolchain prefix (arm-elf- -> arm-elf-gcc.exe)
#TCHAIN_PREFIX := C:/bin/winarm/arm-eabi-gcc-4.5.0/bin/arm-none-eabi-
TCHAIN_PREFIX := arm-none-eabi-


# submodel
SUBMDL := LPC2148
#SUBMDL := LPC1769


# fast GPIO for LPC2148
FAST_GPIO := -DFAST_GPIO


# Target file name (without extension).
TARGET := main


CODE_READ_PROTECTION := -D_CRP_

EXSTRA_C_DEFS := -D$(SUBMDL)
EXTRA_A_DEFS :=

EXSTRA_C_DEFS += $(FAST_GPIO)


#VIC_TYPE := VIC_PL190
#VIC_TYPE := VIC_PL191
#EXSTRA_C_DEFS += -D$(VIC_TYPE)


# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
EXTRAINCDIRS  := ../kernel ../kernel/arm7tdmi


#Additional libraries.
#Support for newlibc-lpc (file: libnewlibc-lpc.a)
#NEWLIBLPC := -lnewlib-lpc
#MATH_LIB := -lm
#CPLUSPLUS_LIB := -lstdc++
EXSTRA_LIB :=

EXTRAOBJS :=


# Optimization level, can be [0, 1, 2, 3, s].
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)

# List C and C++ source files here. (C dependencies are automatically generated.)
# Use file-extension c for "c-only"-files, use file-extension cpp for C++-files
# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.

# ASRC     Assembler source files for Thumb code
# ASRCARM  Assembler source files for ARM code

# SRCOs, SRCO0, SRCO1, SRCO2, SRCO3 for Thumb code with
#  s,      0,     1,     2,     3   optimization level

# SRCARMOs, SRCARMO0, SRCARMO1, SRCARMO2, SRCARMO3 for ARM code with
#    s,         0,       1,         2,        3    optimisation level

# CPPSRCOs, CPPSRCO0, CPPSRCO1, CPPSRCO2, CPPSRCO3 for Thumb C++ code with
#    s,         0,       1,        2,        3     optimisation level

# CPPSRCARMOs, CPPSRCARMO0, CPPSRCARMO1, CPPSRCARMO2, CPPSRCARMO3 for ARM C++ code with
#     s,            0,           1,           2,           3      optimisation level

# APP part
ASRCARM += lpc2148-crt0-os.S
SRCOs += sys_init_lpc21xx.c
SRCOs += $(TARGET).c
SRCOs += arm7tdmi_exception_handler.c
SRCOs += uart0_polling.c

# kernel part
SRCARMOs += ../kernel/kernel.c
SRCARMOs += ../kernel/semaphore.c
SRCARMOs += ../kernel/mutex.c
SRCARMOs += ../kernel/mqueue.c
SRCARMOs += ../kernel/klist.c
SRCARMOs += ../kernel/kspinlock.c
SRCARMOs += ../kernel/kspinsem.c
SRCARMOs += ../kernel/printf.c
SRCARMOs += ../kernel/kprintf.c
SRCARMOs += ../kernel/arm7tdmi/kernel_arm7.c
ASRCARM += ../kernel/arm7tdmi/kernel_arm7_asm.S
ASRCARM += ../kernel/arm7tdmi/arm7-tdmi-atomic.S




#FLASH_TOOL = ULINK
FLASH_TOOL := LPC21ISP


#MCU := cortex-m3
MCU := arm7tdmi-s
THUMB    := -mthumb
#THUMB_IW :=
THUMB_IW = -mthumb-interwork

## Create ROM-Image (final)
RUN_MODE := ROM_RUN
## Create RAM-Image (debugging)
#RUN_MODE = RAM_RUN


# Output format. (can be srec, ihex, binary)
FORMAT := ihex


# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
#DEBUG = stabs
DEBUG := dwarf-2


# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD := -std=gnu99

# Place -D or -U options for C here
CDEFS =  -D$(RUN_MODE)
CDEFS += -D$(SUBMDL)
CDEFS += $(CODE_READ_PROTECTION)
CDEFS += $(EXSTRA_C_DEFS)
CDEFS += -DBUILD_TIMESTAMP=$(BUILD_TIMESTAMP)

# Place -I options here
CINCS =

# Place -D or -U options for ASM here
ADEFS = -D$(RUN_MODE)
ADEFS += -D$(SUBMDL)
ADEFS += $(CODE_READ_PROTECTION)
ADEFS += $(EXSTRA_A_DEFS)

# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
#
# Flags for C and C++ (arm-elf-gcc/arm-elf-g++)
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -Wall
#CFLAGS += -Wcast-align -Wcast-qual -Wimplicit
#CFLAGS += -Wpointer-arith -Wswitch
#CFLAGS += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
CFLAGS += -Wa,-adhlns=$(subst $(suffix $<),.lst,$<)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

CFLAGS += -fomit-frame-pointer

CFLAGS += -fno-exceptions
CFLAGS += -fno-unwind-tables
CFLAGS += -fno-asynchronous-unwind-tables

CFLAGS += -ffunction-sections -fdata-sections

# flags only for C
CONLYFLAGS =
#CONLYFLAGS += -Wstrict-prototypes -Wmissing-declarations
#CONLYFLAGS += -Wmissing-prototypes -Wnested-externs
CONLYFLAGS += $(CSTANDARD)

# flags only for C++ (arm-elf-g++)
CPPFLAGS =
CPPFLAGS += -fno-rtti
CPPFLAGS += -fno-exceptions
CPPFLAGS += -fno-unwind-tables
CPPFLAGS += -fno-asynchronous-unwind-tables

# Assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
##ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
ASFLAGS = $(ADEFS) -Wa,-adhlns=$(<:.S=.lst),-g$(DEBUG)



# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
#     --gc-sections: delete unused sections
#LDFLAGS = -nostartfiles -Wl,-Map=../src/$(TARGET).map,--cref,--gc-sections
LDFLAGS = -nostartfiles
LDFLAGS += -nostdlib
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref,--gc-sections
LDFLAGS += $(NEWLIBLPC) $(MATH_LIB)
LDFLAGS += $(CPLUSPLUS_LIB)

ifdef EXSTRA_LIB
LDFLAGS += $(EXSTRA_LIB)
endif
LDFLAGS += -lc -lgcc

# Set Linker-Script Depending On Selected Memory
ifeq ($(RUN_MODE),RAM_RUN)
LDFLAGS +=-T$(SUBMDL)-RAM.ld
else
LDFLAGS +=-T$(SUBMDL)-ROM.ld
endif


# ---------------------------------------------------------------------------
# Flash-Programming support using lpc21isp by Martin Maurer

# Settings and variables:
#LPC21ISP := lpc21isp
LPC21ISP := lpc21isp-v1.83
LPC21ISP_PORT := com1
LPC21ISP_BAUD := 38400
#LPC21ISP_BAUD := 115200
LPC21ISP_XTAL := 12000
LPC21ISP_FLASHFILE := $(TARGET).hex
# verbose output:
#LPC21ISP_DEBUG := -debug
# enter bootloader via RS232 DTR/RTS (only if hardware supports this
# feature - see Philips AppNote):
#LPC21ISP_CONTROL := -control
#LPC21ISP_ERASE := -erase
LPC21ISP_OPTIONS := -wipe -control


# ---------------------------------------------------------------------------

# Define directories, if needed.

# Define programs and commands.
SHELL = sh

CC      := $(TCHAIN_PREFIX)gcc
CPP     := $(TCHAIN_PREFIX)g++
AR      := $(TCHAIN_PREFIX)ar
OBJCOPY := $(TCHAIN_PREFIX)objcopy
OBJDUMP := $(TCHAIN_PREFIX)objdump
SIZE    := $(TCHAIN_PREFIX)size
NM      := $(TCHAIN_PREFIX)nm

REMOVE := @rm -f
REMOVEDIR := @rm -rf
COPY := @cp

BEEP := beep


# test.exe [ ]
TEST := test


# Define Messages
# English
MSG_COMPILING := Compiling C:
MSG_COMPILING_ARM := "Compiling C (ARM-only):"
MSG_COMPILINGCPP := Compiling C++:
MSG_COMPILINGCPP_ARM := "Compiling C++ (ARM-only):"
MSG_ASSEMBLING := Assembling:
MSG_ASSEMBLING_ARM := "Assembling (ARM-only):"


# Define all object files.
COBJOs      = $(SRCOs:.c=.o)
COBJO0      = $(SRCO0:.c=.o)
COBJO1      = $(SRCO1:.c=.o)
COBJO2      = $(SRCO2:.c=.o)
COBJO3      = $(SRCO3:.c=.o)
COBJARMOs   = $(SRCARMOs:.c=.o)
COBJARMO0   = $(SRCARMO0:.c=.o)
COBJARMO1   = $(SRCARMO1:.c=.o)
COBJARMO2   = $(SRCARMO2:.c=.o)
COBJARMO3   = $(SRCARMO3:.c=.o)
CPPOBJOs    = $(CPPSRCOs:.cpp=.o)
CPPOBJO0    = $(CPPSRCO0:.cpp=.o)
CPPOBJO1    = $(CPPSRCO1:.cpp=.o)
CPPOBJO2    = $(CPPSRCO2:.cpp=.o)
CPPOBJO3    = $(CPPSRCO3:.cpp=.o)
CPPOBJARMOs = $(CPPSRCARMOs:.cpp=.o)
CPPOBJARMO0 = $(CPPSRCARMO0:.cpp=.o)
CPPOBJARMO1 = $(CPPSRCARMO1:.cpp=.o)
CPPOBJARMO2 = $(CPPSRCARMO2:.cpp=.o)
CPPOBJARMO3 = $(CPPSRCARMO3:.cpp=.o)
AOBJ      = $(ASRC:.S=.o)
AOBJARM   = $(ASRCARM:.S=.o)

ALL_OBJ =  $(AOBJARM) $(AOBJ)
ALL_OBJ += $(COBJARMOs) $(COBJARMO0) $(COBJARMO1) $(COBJARMO2) $(COBJARMO3)
ALL_OBJ += $(COBJOs) $(COBJO0) $(COBJO1) $(COBJO2) $(COBJO2) $(COBJO3)
ALL_OBJ += $(CPPOBJARMOs) $(CPPOBJARMO0) $(CPPOBJARMO1) $(CPPOBJARMO2) $(CPPOBJARMO3)
ALL_OBJ += $(CPPOBJOs) $(CPPOBJO0) $(CPPOBJO1) $(CPPOBJO2) $(CPPOBJO3)


ALL_SRC =  $(ASRC) $(ASRCARM)
ALL_SRC += $(SRCOs) $(SRCO0) $(SRCO1) $(SRCO2) $(SRCO3)
ALL_SRC += $(SRCARMOs) $(SRCARMO0) $(SRCARMO1) $(SRCARMO2) $(SRCARMO3)
ALL_SRC += $(CPPSRCOs) $(CPPSRCO0) $(CPPSRCO1) $(CPPSRCO2) $(CPPSRCO3)
ALL_SRC += $(CPPSRCARMOs) $(CPPSRCARMO0) $(CPPSRCARMO1) $(CPPSRCARMO2) $(CPPSRCARMO3)

# Define all listing files.
LST =  $(ASRC:.S=.lst) $(ASRCARM:.S=.lst)
LST += $(SRCOs:.c=.lst) $(SRCARMOs:.c=.lst)
LST += $(SRCO0:.c=.lst) $(SRCARMO0:.c=.lst)
LST += $(SRCO1:.c=.lst) $(SRCARMO1:.c=.lst)
LST += $(SRCO2:.c=.lst) $(SRCARMO2:.c=.lst)
LST += $(SRCO3:.c=.lst) $(SRCARMO3:.c=.lst)
LST += $(CPPSRCOs:.cpp=.lst) $(CPPSRCARMOs:.cpp=.lst)
LST += $(CPPSRCO0:.cpp=.lst) $(CPPSRCARMO0:.cpp=.lst)
LST += $(CPPSRCO1:.cpp=.lst) $(CPPSRCARMO1:.cpp=.lst)
LST += $(CPPSRCO2:.cpp=.lst) $(CPPSRCARMO2:.cpp=.lst)
LST += $(CPPSRCO3:.cpp=.lst) $(CPPSRCARMO3:.cpp=.lst)

# Compiler flags to generate dependency files.
### GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mcpu=$(MCU) $(THUMB_IW) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mcpu=$(MCU) $(THUMB_IW) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
#all: begin gccversion update_timestamp sizebefore build sizeafter finished end
all: begin gccversion sizebefore build sizeafter end

build: objdir elf hex lss sym dis

elf: $(TARGET).elf
lss: $(TARGET).lss
sym: $(TARGET).sym
dis: $(TARGET).dis
#hex: $(TARGET).hex
hex: $(TARGET).elf
	@echo
	@echo Creating load file for Flash: $(TARGET).$@
	$(OBJCOPY) -O $(FORMAT) $< $(TARGET).$@



ifdef OBJDIR
objdir:
	@if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
else
objdir:

endif


# Eye candy.
begin:
	@echo
	@echo -------- begin --------

end:
	@echo --------  end  --------
	@echo


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf
sizebefore:
	@if [ -f $(TARGET).elf ]; then echo; echo Size before:; $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then echo; echo Size after:; $(ELFSIZE); echo; fi


# Display compiler version information.
gccversion :
	@$(CC) --version


program: $(TARGET).hex
	@echo
	@$(LPC21ISP) $(LPC21ISP_OPTIONS) $(LPC21ISP_FLASHFILE) $(LPC21ISP_PORT) $(LPC21ISP_BAUD) $(LPC21ISP_XTAL)



# Create extended listing file from ELF output file.
# testing: option -C%.lss: %.elf
%.lss: %.elf
	@echo
	@echo Creating Extended Listing: $@
	$(OBJDUMP) -h -S -C $< > $@

# Create full disasembly from ELF output file.
%.dis: %.elf
	@echo
	@echo Creating Full Disasembly: $@
	$(OBJDUMP) -D $< > $@


# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo Creating Symbol Table: $@
	$(NM) -n $< > $@


# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(ALL_OBJ)
%.elf:  $(ALL_OBJ)
	@echo
	@echo Linking: $@
	$(CC) $(THUMB) $(ALL_CFLAGS) $(ALL_OBJ) $(EXTRAOBJS) --output $@ $(LDFLAGS)


# Link: create .a output file from object files.
.SECONDARY : $(TARGETLIB).a
.PRECIOUS : $(ALL_OBJ)
%.a:  $(ALL_OBJ)
	@echo
	@echo Linking: $@
	cd $(OBJDIR); $(AR) rc $(TARGETLIB).a $(ALL_OBJ)



# Optimization -Os

# Compile: create object files from C source files. ARM/Thumb
$(COBJOs) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(THUMB) $(ALL_CFLAGS) -Os $(CONLYFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARMOs) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING_ARM) $<
	$(CC) -c $(ALL_CFLAGS) -Os $(CONLYFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJOs) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP) $<
	$(CPP) -c $(THUMB) $(ALL_CFLAGS) -Os $(CPPFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM-only
$(CPPOBJARMOs) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP_ARM) $<
	$(CPP) -c $(ALL_CFLAGS) -Os $(CPPFLAGS) $< -o $@


# Optimization -O0

# Compile: create object files from C source files. ARM/Thumb
$(COBJO0) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(THUMB) $(ALL_CFLAGS) -O0 $(CONLYFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARMO0) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING_ARM) $<
	$(CC) -c $(ALL_CFLAGS) -O0 $(CONLYFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJO0) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP) $<
	$(CPP) -c $(THUMB) $(ALL_CFLAGS) -O0 $(CPPFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM-only
$(CPPOBJARMO0) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP_ARM) $<
	$(CPP) -c $(ALL_CFLAGS) -O0 $(CPPFLAGS) $< -o $@


# Optimization -O1

# Compile: create object files from C source files. ARM/Thumb
$(COBJO1) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(THUMB) $(ALL_CFLAGS) -O1 $(CONLYFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARMO1) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING_ARM) $<
	$(CC) -c $(ALL_CFLAGS) -O1 $(CONLYFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJO1) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP) $<
	$(CPP) -c $(THUMB) $(ALL_CFLAGS) -O1 $(CPPFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM-only
$(CPPOBJARMO1) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP_ARM) $<
	$(CPP) -c $(ALL_CFLAGS) -O1 $(CPPFLAGS) $< -o $@


# Optimization -O2

# Compile: create object files from C source files. ARM/Thumb
$(COBJO2) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(THUMB) $(ALL_CFLAGS) -O2 $(CONLYFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARMO2) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING_ARM) $<
	$(CC) -c $(ALL_CFLAGS) -O2 $(CONLYFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJO2) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP) $<
	$(CPP) -c $(THUMB) $(ALL_CFLAGS) -O2 $(CPPFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM-only
$(CPPOBJARMO2) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP_ARM) $<
	$(CPP) -c $(ALL_CFLAGS) -O2 $(CPPFLAGS) $< -o $@



# Optimization -O3

# Compile: create object files from C source files. ARM/Thumb
$(COBJO3) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(THUMB) $(ALL_CFLAGS) -O3 $(CONLYFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARMO3) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING_ARM) $<
	$(CC) -c $(ALL_CFLAGS) -O3 $(CONLYFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJO3) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP) $<
	$(CPP) -c $(THUMB) $(ALL_CFLAGS) -O3 $(CPPFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM-only
$(CPPOBJARMO3) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP_ARM) $<
	$(CPP) -c $(ALL_CFLAGS) -O3 $(CPPFLAGS) $< -o $@


# Assemble: create object files from assembler source files. ARM/Thumb
$(AOBJ) : %.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(THUMB) $(ALL_ASFLAGS) $< -o $@


# Assemble: create object files from assembler source files. ARM-only
$(AOBJARM) : %.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING_ARM) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean: begin clean_list end


clean_list :
	@echo
	@echo Cleaning project:
ifeq ($(TARGET)__XXX__,__XXX__)
else
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).a90
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(TARGET).dis
endif
ifeq ($(TARGETLIB)__XXX__,__XXX__)
else
	$(REMOVE) $(TARGETLIB).obj
	$(REMOVE) $(TARGETLIB).elf
	$(REMOVE) $(TARGETLIB).map
	$(REMOVE) $(TARGETLIB).obj
	$(REMOVE) $(TARGETLIB).a90
	$(REMOVE) $(TARGETLIB).sym
	$(REMOVE) $(TARGETLIB).lnk
	$(REMOVE) $(TARGETLIB).lss
	$(REMOVE) $(TARGETLIB).dis
endif
	$(REMOVE) $(ALL_OBJ)
	$(REMOVE) $(LST)
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) $(SRCOs:.c=.s)
	$(REMOVE) $(SRCO0:.c=.s)
	$(REMOVE) $(SRCO1:.c=.s)
	$(REMOVE) $(SRCO2:.c=.s)
	$(REMOVE) $(SRCO3:.c=.s)
	$(REMOVE) $(SRCOs:.c=.d)
	$(REMOVE) $(SRCO0:.c=.d)
	$(REMOVE) $(SRCO1:.c=.d)
	$(REMOVE) $(SRCO2:.c=.d)
	$(REMOVE) $(SRCO3:.c=.d)
	$(REMOVE) $(SRCARMOs:.c=.s)
	$(REMOVE) $(SRCARMO0:.c=.s)
	$(REMOVE) $(SRCARMO1:.c=.s)
	$(REMOVE) $(SRCARMO2:.c=.s)
	$(REMOVE) $(SRCARMO3:.c=.s)
	$(REMOVE) $(SRCARMOs:.c=.d)
	$(REMOVE) $(SRCARMO0:.c=.d)
	$(REMOVE) $(SRCARMO1:.c=.d)
	$(REMOVE) $(SRCARMO2:.c=.d)
	$(REMOVE) $(SRCARMO3:.c=.d)
	$(REMOVE) $(CPPSRCOs:.cpp=.s)
	$(REMOVE) $(CPPSRCO0:.cpp=.s)
	$(REMOVE) $(CPPSRCO1:.cpp=.s)
	$(REMOVE) $(CPPSRCO2:.cpp=.s)
	$(REMOVE) $(CPPSRCO3:.cpp=.s)
	$(REMOVE) $(CPPSRCOs:.cpp=.d)
	$(REMOVE) $(CPPSRCO0:.cpp=.d)
	$(REMOVE) $(CPPSRCO1:.cpp=.d)
	$(REMOVE) $(CPPSRCO2:.cpp=.d)
	$(REMOVE) $(CPPSRCO3:.cpp=.d)
	$(REMOVE) $(CPPSRCARMOs:.cpp=.s)
	$(REMOVE) $(CPPSRCARMO0:.cpp=.s)
	$(REMOVE) $(CPPSRCARMO1:.cpp=.s)
	$(REMOVE) $(CPPSRCARMO2:.cpp=.s)
	$(REMOVE) $(CPPSRCARMO3:.cpp=.s)
	$(REMOVE) $(CPPSRCARMOs:.cpp=.d)
	$(REMOVE) $(CPPSRCARMO0:.cpp=.d)
	$(REMOVE) $(CPPSRCARMO1:.cpp=.d)
	$(REMOVE) $(CPPSRCARMO2:.cpp=.d)
	$(REMOVE) $(CPPSRCARMO3:.cpp=.d)
	$(REMOVE) .dep/*
	$(REMOVEDIR) .dep
	$(REMOVEDIR) $(OBJDIR)

Debug: all
Release: all
cleanDebug: clean
cleanRelease: clean

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion build buildlib a elf hex \
lss sym dis clean clean_list program update_buildnum update_timestamp all_timestamp backup \
Debug Release cleanDebug cleanRelease

