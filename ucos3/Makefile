# General Makefile for linux
# Created by: Jean-Christophe Dubois

PROJ_NAME = OS

BOARD = MCIMX6UL-EVK
ARCH = ARM-Cortex-A
SUBARCH = ARMv7-A
TOOLCHAIN = GNU
CROSS_COMPILE = arm-none-eabi-
PHY = Generic
MAC = MACNet

###############################################################################
##  Standard commands
###############################################################################

CC=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/$(CROSS_COMPILE)gcc
AS=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/$(CROSS_COMPILE)gcc
AR=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/$(CROSS_COMPILE)ar
LD=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/$(CROSS_COMPILE)ld
NM=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/$(CROSS_COMPILE)nm
OBJDUMP=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/$(CROSS_COMPILE)@objdump
OBJCOPY=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/bin/$(CROSS_COMPILE)@objcopy
ECHO=@echo
MKDIR=@mkdir -p
RMDIR=@rmdir --ignore-fail-on-non-empty
DEL=@/bin/rm -f
CP=@cp

###############################################################################
##  Source paths
###############################################################################

PROJ_ROOT = .
APP_ROOT = $(PROJ_ROOT)/NXP/$(BOARD)/Ping/OS3
APP2_ROOT = $(PROJ_ROOT)/NXP/$(BOARD)/Ping
BSP_ROOT = $(PROJ_ROOT)/NXP/BSP
OS_ROOT = $(PROJ_ROOT)/Micrium/Software/uCOS-III/Source
OS_PORT_ROOT = $(OS_ROOT)/../Ports/$(ARCH)/$(SUBARCH)
CPU_ROOT = $(PROJ_ROOT)/Micrium/Software/uC-CPU
LIB_ROOT = $(PROJ_ROOT)/Micrium/Software/uC-LIB
IP_ROOT = $(PROJ_ROOT)/Micrium/Software/uC-TCP-IP/Source
IPv4_ROOT = $(IP_ROOT)/../IP/IPv4
IPv6_ROOT = $(IP_ROOT)/../IP/IPv6
IF_ROOT = $(IP_ROOT)/../IF
PHY_ROOT = $(IP_ROOT)/../Dev/Ether/PHY/$(PHY)
MAC_ROOT = $(IP_ROOT)/../Dev/Ether/$(MAC)
COMMON_ROOT = $(PROJ_ROOT)/Micrium/Software/uC-Common
CPU_ROOT = $(PROJ_ROOT)/Micrium/Software/uC-CPU
DHCP_ROOT = $(PROJ_ROOT)/Micrium/Software/uC-DHCPc/Source
DHCP_OS_ROOT = $(DHCP_ROOT)/../OS/uCOS-III
PRINTF_ROOT = $(PROJ_ROOT)/printf
OUTPUT = $(PROJ_ROOT)/output

###############################################################################
##  Commands Parameters
###############################################################################

CFLAGS    =  -march=armv7-a  -fno-stack-protector -nostdlib -fno-builtin -fno-mudflap -U_FORTIFY_SOURCE 
CINCLUDES = -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/include/ -I$(APP_ROOT) -I$(OS_ROOT) -I$(OS_PORT_ROOT)/$(TOOLCHAIN)
CINCLUDES += -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/plugin/include/ -I$(CPU_ROOT) -I$(CPU_ROOT)/$(ARCH)/$(SUBARCH)/$(TOOLCHAIN)
CINCLUDES += -I$(APP_ROOT)/.. -I$(LIB_ROOT) -I$(IP_ROOT)/.. -I$(OS_ROOT)/.. -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/plugin/include/
CINCLUDES += -I$(COMMON_ROOT) -I$(COMMON_ROOT)/KAL -I$(BSP_ROOT)/$(BOARD) -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/plugin/include/
CINCLUDES += -I$(DHCP_ROOT)/.. -I$(PRINTF_ROOT) -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/plugin/include/ -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/lib/
AINCLUDES = -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/plugin/include/ -I$(APP_ROOT),-I$(OS_ROOT),-I$(OS_PORT_ROOT)/$(TOOLCHAIN)
ASFLAGS   = -mcpu=armv7-a -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/plugin/include/
# LDFLAGS   = -nostdlib \
# -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/include \
# -I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/lib/ 
# -isysroot=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1
LDFLAGS   = -nostdlib -Wl,-Tlinker.ld  \
-Wl,--unresolved-symbols=ignore-all \
-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/include \
-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/lib/ \
-isysroot=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1

OS_SRC      := $(shell cd $(OS_ROOT); ls os_*.c)
OS_PORT_SRC := $(shell cd $(OS_PORT_ROOT); ls *.c) os_cpu_a_vfp-none.S
APP_SRC     := $(shell cd $(APP_ROOT); ls *.c)
APP2_SRC    := $(shell cd $(APP2_ROOT); ls *.c)
IP_SRC      := $(shell cd $(IP_ROOT); ls *.c)
IPv4_SRC    := $(shell cd $(IPv4_ROOT); ls *.c)
IPv6_SRC    := $(shell cd $(IPv6_ROOT); ls *.c)
IF_SRC      := $(shell cd $(IF_ROOT); ls *.c)
PHY_SRC     := $(shell cd $(PHY_ROOT); ls *.c)
MAC_SRC     := $(shell cd $(MAC_ROOT); ls *.c)
LIB_SRC     := $(shell cd $(LIB_ROOT); ls *.c)
LIB_SRC     := $(shell cd $(LIB_ROOT); ls *.c)
CPU_SRC     := cpu_core.c cpu_a.S
BSP_SRC     := $(shell cd $(BSP_ROOT)/$(BOARD)/; ls *.c)
BSP_INT_SRC := bsp_int_armv7a_gicv2.c
BSP_START_SRC := cstartup.S ivt.c
DHCP_SRC    := $(shell cd $(DHCP_ROOT); ls *.c)
DHCP_OS_SRC := $(shell cd $(DHCP_OS_ROOT); ls *.c)
PRINTF_SRC := $(shell cd $(PRINTF_ROOT); ls *.c)
COMMON_SRC  := kal.c

OBJS_C      :=$(addprefix $(OS_ROOT)/,  $(patsubst %.c,%.o,$(filter %.c,$(OS_SRC))))   \
              $(addprefix $(OS_PORT_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(OS_PORT_SRC))))  \
              $(addprefix $(IP_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(IP_SRC))))  \
              $(addprefix $(PHY_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(PHY_SRC))))  \
              $(addprefix $(MAC_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(MAC_SRC))))  \
              $(addprefix $(IPv4_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(IPv4_SRC))))  \
              $(addprefix $(IPv6_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(IPv6_SRC))))  \
              $(addprefix $(IF_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(IF_SRC))))  \
              $(addprefix $(LIB_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(LIB_SRC))))  \
              $(addprefix $(CPU_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(CPU_SRC))))  \
              $(addprefix $(BSP_ROOT)/$(BOARD)/, $(patsubst %.c,%.o,$(filter %.c,$(BSP_SRC))))  \
              $(addprefix $(BSP_ROOT)/Interrupts/, $(patsubst %.c,%.o,$(filter %.c,$(BSP_INT_SRC))))  \
              $(addprefix $(COMMON_ROOT)/KAL/uCOS-III/, $(patsubst %.c,%.o,$(filter %.c,$(COMMON_SRC))))  \
              $(addprefix $(APP2_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(APP2_SRC))))  \
              $(addprefix $(DHCP_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(DHCP_SRC))))  \
              $(addprefix $(DHCP_OS_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(DHCP_OS_SRC))))  \
              $(addprefix $(PRINTF_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(PRINTF_SRC))))  \
              $(addprefix $(BSP_ROOT)/Startup/MCIMX6G2/$(TOOLCHAIN)/, $(patsubst %.c,%.o,$(filter %.c,$(BSP_START_SRC))))  \
              $(addprefix $(APP_ROOT)/, $(patsubst %.c,%.o,$(filter %.c,$(APP_SRC))))
OBJS_S      :=$(addprefix $(OS_ROOT)/,  $(patsubst %.S,%.o,$(filter %.S,$(OS_SRC))))   \
              $(addprefix $(OS_PORT_ROOT)/$(TOOLCHAIN)/, $(patsubst %.S,%.o,$(filter %.S,$(OS_PORT_SRC))))  \
              $(addprefix $(CPU_ROOT)/$(ARCH)/$(SUBARCH)/$(TOOLCHAIN)/, $(patsubst %.S,%.o,$(filter %.S,$(CPU_SRC))))  \
              $(addprefix $(BSP_ROOT)/Startup/MCIMX6G2/$(TOOLCHAIN)/, $(patsubst %.S,%.o,$(filter %.S,$(BSP_START_SRC))))  \
              $(addprefix $(APP_ROOT)/, $(patsubst %.S,%.o,$(filter %.S,$(APP_SRC))))
OBJS_ALL    :=$(addprefix $(OUTPUT)/obj/,$(notdir $(OBJS_C)) $(notdir $(OBJS_S)))

###############################################################################
##  Pattern Rules
###############################################################################

CLFLAGS    = -fno-exceptions -mcpu=cortex-a7 -Wno-conversion -nostdinc -fno-stack-protector -nostdlib -fno-builtin -U_FORTIFY_SOURCE -fsanitize-coverage=trace-pc-guard \
			--target=armv7m-unknown-none-eabi -isysroot=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1\
			--sysroot=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi \
			-I/root/fuzz/kernel/ucos3/Micrium/Software/uC-TCP-IP/IP/IPv4/ \
			-I/root/fuzz/kernel/ucos3/Micrium/Software/uC-TCP-IP/Source/ \
			-I/root/fuzz/kernel/ucos3/Micrium/Software/uC-TCP-IP/IP/IPv6/ \
			-I/root/fuzz/kernel/ucos3/Micrium/Software/uC-DHCPc/Source/ \
			-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/include \
			-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1 \
			-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/lib/ \
			-Wl,--unresolved-symbols=ignore-all
			
%.o: %.c
	$(ECHO) '$(PROC)/GNU C Compiler building $(notdir $<)'
	$(MKDIR) $(OUTPUT)
	$(MKDIR) $(OUTPUT)/obj
	$(MKDIR) $(OUTPUT)/lst	
	clang -c -g -O0 $(CLFLAGS) $(CINCLUDES) -o "$(OUTPUT)/obj/$(notdir $@)" "$<"

%.o: %.asm
	$(ECHO) '$(PROC)/GNU ASM Compiler building $(notdir $<)'
	$(MKDIR) $(OUTPUT)
	$(MKDIR) $(OUTPUT)/obj
	$(AS) -g -Os $(ASFLAGS) $(AINCLUDES) -o "$(OUTPUT)/obj/$(notdir $@)" "$<"

%.o: %.S
	$(ECHO) '$(PROC)/GNU C (ASM Pre-processing) Compiler building $(notdir $<)'
	$(MKDIR) $(OUTPUT)
	$(MKDIR) $(OUTPUT)/obj
	$(MKDIR) $(OUTPUT)/lst
	$(CC) -c -g -Os $(CFLAGS) $(CINCLUDES) -Wa,-alh="$(OUTPUT)/lst/$(notdir $(patsubst %.s,%.lst,$<))",-L,$(AINCLUDES) -o "$(OUTPUT)/obj/$(notdir $@)" "$<"

###############################################################################
##  Build Rules
###############################################################################

all: build

build: $(OBJS_C) $(OBJS_ASM) $(OBJS_S)
	$(ECHO) '$(PROC)/GNU C Linker building target $(PROJ_NAME)'
	$(MKDIR) $(OUTPUT)
	$(CC)  $(LDFLAGS) -o "$(OUTPUT)/$(PROJ_NAME).elf" $(OBJS_ALL) -lm /opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/lib/libc.a ./libclang_rt.builtins-armv7.a /opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/libgcc.a

# $(LD) $(LDFLAGS) -o "$(OUTPUT)/$(PROJ_NAME).elf" $(OBJS_ALL) -lm /opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/lib/libc.a ./libclang_rt.builtins-armv7.a /opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/lib/gcc/arm-none-eabi/10.3.1/libgcc.a

	$(CP) "$(OUTPUT)/$(PROJ_NAME).elf" "$(PROJ_ROOT)/$(PROJ_NAME).elf"
	$(ECHO) 'Target $(PROJ_NAME) completed.'

clean:
	-$(DEL) $(OUTPUT)/obj/*.o $(OUTPUT)/lst/*.lst
	-$(DEL) "$(OUTPUT)/$(PROJ_NAME).elf"
	-$(DEL) "$(PROJ_ROOT)/$(PROJ_NAME).elf"
	-$(RMDIR) $(OUTPUT)/obj
	-$(RMDIR) $(OUTPUT)/lst
	-$(RMDIR) $(OUTPUT)
	$(ECHO) 'Target $(PROJ_NAME) clean.'
