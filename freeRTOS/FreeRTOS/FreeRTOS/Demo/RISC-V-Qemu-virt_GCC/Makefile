CROSS   = riscv32-unknown-elf-
#CC      = $(CROSS)gcc
#OBJCOPY = $(CROSS)objcopy
#ARCH    = $(CROSS)ar

CC=clang
OBJCOPY=llvm-objcopy
ARCH=llvm-ar

BUILD_DIR       = build
RTOS_SOURCE_DIR = $(abspath ../../Source)
DEMO_SOURCE_DIR = $(abspath ../Common/Minimal)

CPPFLAGS = \
	-D__riscv_float_abi_soft -fsanitize-undefined-trap-on-error \
	-DportasmHANDLE_INTERRUPT=handle_trap \
	--sysroot=/usr/local/riscv32-unknown-elf \
	--target=riscv32-unknown-elf \
	-I . -I ../Common/include \
	-I $(RTOS_SOURCE_DIR)/include \
	-I /usr/local/riscv32-unknown-elf/include/ \
	-I /usr/local/riscv32-unknown-elf/lib/ \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/RISC-V \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions

CPPAFLAGS = \
	-D__riscv_float_abi_soft \
	-DportasmHANDLE_INTERRUPT=handle_trap \
	-I . -I ../Common/include \
	-I $(RTOS_SOURCE_DIR)/include \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/RISC-V \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions

CFLAGS  = -mabi=ilp32 -mcmodel=medany \
	-Wall \
	-fmessage-length=0 \
	-ffunction-sections \
	-fdata-sections \
	-fno-builtin-printf
ASFLAGS = -march=rv32ima -mabi=ilp32 -mcmodel=medany
LDFLAGS = -nostartfiles -Tfake_rom.lds \
	-Xlinker --gc-sections \
	-I /usr/local/riscv32-unknown-elf/include/ \
	-Xlinker --defsym=__stack_size=1935000

ifeq ($(DEBUG), 1)
    CFLAGS += -Og -ggdb3 -O0
else
    CFLAGS += -Og -ggdb3 -O0
endif

SRCS = main.c main_blinky.c riscv-virt.c ns16550.c \
	$(DEMO_SOURCE_DIR)/EventGroupsDemo.c \
	$(DEMO_SOURCE_DIR)/TaskNotify.c \
	$(DEMO_SOURCE_DIR)/TimerDemo.c \
	$(DEMO_SOURCE_DIR)/blocktim.c \
	$(DEMO_SOURCE_DIR)/dynamic.c \
	$(DEMO_SOURCE_DIR)/recmutex.c \
	$(RTOS_SOURCE_DIR)/event_groups.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/stream_buffer.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/timers.c \
	$(RTOS_SOURCE_DIR)/portable/MemMang/heap_4.c \
	$(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/port.c

ASMS = start.S \
	$(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/portASM.S

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o) $(ASMS:%.S=$(BUILD_DIR)/%.o)
DEPS = $(SRCS:%.c=$(BUILD_DIR)/%.d) $(ASMS:%.S=$(BUILD_DIR)/%.d)

$(BUILD_DIR)/RTOSDemo.axf: $(OBJS) fake_rom.lds Makefile
	$(CROSS)gcc -g -O0 $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(@D)
	$(CC) -g -O0 $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile
	@mkdir -p $(@D)
	$(CROSS)gcc -g -O0 $(CPPAFLAGS) $(ASFLAGS) -MMD -MP -c $< -o $@
# $(CC) $(CPPFLAGS) $(ASFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
