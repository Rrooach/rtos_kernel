ARM GAS  /tmp/cc1lgKik.s 			page 1


   1              	# 1 "NXP/BSP/Startup/MCIMX6G2/GNU/cstartup.S"
   1              	
   1              	...
   0              	
   0              	
   2              	/****************************************************************************
   3              	 *                    EXCEPTION VECTORS & STARTUP CODE
   4              	 *
   5              	 * File      : cstartup.s
   6              	 * For       : Cortex-A7 on the Freescale MCIMX6G
   7              	 * Toolchain : GNU
   8              	 ****************************************************************************/
   9              	
  10              	
  11              	/****************************************************************************
  12              	 *                    MACROS AND DEFINIITIONS
  13              	 ****************************************************************************/
  14              	
  15              	#define CPSR_MODE_USER                                  0x00000010
  16              	#define CPSR_MODE_FIQ                                   0x00000011
  17              	#define CPSR_MODE_IRQ                                   0x00000012
  18              	#define CPSR_MODE_SUPERVISOR                            0x00000013
  19              	#define CPSR_MODE_MONITOR                               0x00000016
  20              	#define CPSR_MODE_ABORT                                 0x00000017
  21              	#define CPSR_MODE_UNDEFINED                             0x0000001b
  22              	#define CPSR_MODE_SYSTEM                                0x0000001f
  23              	
  24              	    .section .entry, "ax", %progbits
  25              	    .globl _start_vect
  26              	    .globl _start
  27              	_start_vect:
  28              	_start:
  29              	
  30              	/****************************************************************************
  31              	 *                    Exception Vector Table
  32              	 ****************************************************************************/
  33              	
  34 0000 18F09FE5 	    ldr pc, __reset
  35 0004 18F09FE5 	    ldr pc, __undefined_instruction
  36 0008 18F09FE5 	    ldr pc, __software_interrupt
  37 000c 18F09FE5 	    ldr pc, __prefetch_abort
  38 0010 18F09FE5 	    ldr pc, __data_abort
  39 0014 18F09FE5 	    ldr pc, __not_used
  40 0018 18F09FE5 	    ldr pc, __irq
  41 001c 18F09FE5 	    ldr pc, __fiq
  42              	
  43              	__reset:
  44 0020 00000000 	    .word _reset
  45              	__undefined_instruction:
  46 0024 00000000 	    .word OS_CPU_ARM_ExceptUndefInstrHndlr
  47              	__software_interrupt:
  48 0028 00000000 	    .word OS_CPU_ARM_ExceptSwiHndlr
  49              	__prefetch_abort:
  50 002c 00000000 	    .word OS_CPU_ARM_ExceptPrefetchAbortHndlr
  51              	__data_abort:
  52 0030 00000000 	    .word OS_CPU_ARM_ExceptDataAbortHndlr
  53              	__not_used:
ARM GAS  /tmp/cc1lgKik.s 			page 2


  54 0034 00000000 	    .word 0
  55              	__irq:
  56 0038 00000000 	    .word OS_CPU_ARM_ExceptIrqHndlr
  57              	__fiq:
  58 003c 00000000 	    .word OS_CPU_ARM_ExceptFiqHndlr
  59              	    .global _end_vect
  60              	_end_vect:
  61              	
  62              	    /* Exception stacks */
  63              	__user_stack_end:
  64 0040 00000000 	    .word _user_stack_end
  65              	__svc_stack_end:
  66 0044 00000000 	    .word _svc_stack_end
  67              	__und_stack_end:
  68 0048 00000000 	    .word _und_stack_end
  69              	__abt_stack_end:
  70 004c 00000000 	    .word _abt_stack_end
  71              	__irq_stack_end:
  72 0050 00000000 	    .word _irq_stack_end
  73              	__fiq_stack_end:
  74 0054 00000000 	    .word _fiq_stack_end
  75              	__start_vect:
  76 0058 00000000 	    .word _start_vect
  77              	__gic_start:
  78 005c 00000000 	    .word _gic_start
  79              	
  80              	    /* bss adresses */
  81              	__bss_start:
  82 0060 00000000 	    .word _bss_start
  83              	__bss_end:
  84 0064 00000000 	    .word _bss_end
  85              	
  86              	    .globl _reset
  87              	_reset:
  88              	
  89              	/****************************************************************************
  90              	 *                    STACK POINTER INITIALIZATION
  91              	 ****************************************************************************/
  92              	
  93              	    /* Disable IRQ & FIQ */
  94 0068 C0000CF1 	    cpsid if
  95              	
  96              	    /* Set the exception vector (VBAR) */
  97 006c 1C001FE5 	    ldr r0, __start_vect
  98 0070 100F0CEE 	    mcr p15, 0, r0, c12, c0, 0
  99              	
 100              	    /* load supervisor stack */
 101 0074 130002F1 	    cps #CPSR_MODE_SUPERVISOR
 102 0078 3CD01FE5 	    ldr sp, __svc_stack_end
 103              	
 104              	    /* load undefined instruction exception stack */
 105 007c 1B0002F1 	    cps #CPSR_MODE_UNDEFINED
 106 0080 40D01FE5 	    ldr sp, __und_stack_end
 107              	
 108              	    /* load data abort exception stack */
 109 0084 170002F1 	    cps #CPSR_MODE_ABORT
 110 0088 44D01FE5 	    ldr sp, __abt_stack_end
ARM GAS  /tmp/cc1lgKik.s 			page 3


 111              	
 112              	    /* load interrupt stack */
 113 008c 120002F1 	    cps #CPSR_MODE_IRQ
 114 0090 48D01FE5 	    ldr sp, __irq_stack_end
 115              	
 116              	    /* load FIQ stack */
 117 0094 110002F1 	    cps #CPSR_MODE_FIQ
 118 0098 4CD01FE5 	    ldr sp, __fiq_stack_end
 119              	
 120              	    /* load SYS stack */
 121 009c 1F0002F1 	    cps #CPSR_MODE_SYSTEM
 122 00a0 68D01FE5 	    ldr sp, __user_stack_end
 123              	
 124              	/****************************************************************************
 125              	 *                    Zero-out bss section
 126              	 ****************************************************************************/
 127              	
 128 00a4 0000A0E3 	    mov r0, #0
 129 00a8 50101FE5 	    ldr r1, __bss_start
 130 00ac 50201FE5 	    ldr r2, __bss_end
 131              	
 132              	_bss_zero:
 133 00b0 040081E4 	    str r0, [r1], #4
 134 00b4 020051E1 	    cmp r1, r2
 135 00b8 FCFFFFBA 	    blt _bss_zero
 136              	
 137              	/****************************************************************************
 138              	 *                    Jump to main
 139              	 ****************************************************************************/
 140              	
 141 00bc FEFFFFEA 	    b main
 142              	    /* We should never reach here */
 143 00c0 FEFFFFEA 	    b .
