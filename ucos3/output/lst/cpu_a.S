ARM GAS  /tmp/ccwZrbJW.s 			page 1


   1              	# 1 "Micrium/Software/uC-CPU/ARM-Cortex-A/ARMv7-A/GNU/cpu_a.S"
   1              	@**************************************************************************************************
   1              	...
   0              	
   0              	
   2              	@                                               uC/CPU
   3              	@                                    CPU CONFIGURATION & PORT LAYER
   4              	@
   5              	@                    Copyright 2004-2020 Silicon Laboratories Inc. www.silabs.com
   6              	@
   7              	@                                 SPDX-License-Identifier: APACHE-2.0
   8              	@
   9              	@               This software is subject to an open source license and is distributed by
  10              	@                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
  11              	@                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
  12              	@
  13              	@**************************************************************************************************
  14              	
  15              	@**************************************************************************************************
  16              	@
  17              	@                                            CPU PORT FILE
  18              	@
  19              	@                                               ARMv7-A
  20              	@                                           GNU C Compiler
  21              	@
  22              	@ Filename : cpu_a.S
  23              	@ Version  : v1.32.00
  24              	@**************************************************************************************************
  25              	
  26              	
  27              	@**************************************************************************************************
  28              	@                                           .global FUNCTIONS
  29              	@**************************************************************************************************
  30              	
  31              	    .global  CPU_SR_Save
  32              	    .global  CPU_SR_Restore
  33              	
  34              	    .global  CPU_IntDis
  35              	    .global  CPU_IntEn
  36              	
  37              	    .global  CPU_WaitForInt
  38              	    .global  CPU_WaitForEvent
  39              	
  40              	    .global  CPU_CntLeadZeros
  41              	    .global  CPU_CntTrailZeros
  42              	
  43              	
  44              	@**************************************************************************************************
  45              	@                                      CODE GENERATION DIRECTIVES
  46              	@**************************************************************************************************
  47              	
  48              	     .code 32
  49              	
  50              	
  51              	@**************************************************************************************************
  52              	@                                     ENABLE & DISABLE INTERRUPTS
  53              	@
ARM GAS  /tmp/ccwZrbJW.s 			page 2


  54              	@ Description : Disable/Enable IRQs & FIQs.
  55              	@
  56              	@ Prototypes  : void  CPU_IntEn (void)@
  57              	@               void  CPU_IntDis(void)@
  58              	@**************************************************************************************************
  59              	
  60              	    .type   CPU_IntDis, %function
  61              	CPU_IntDis:
  62 0000 C0000CF1 	        CPSID   IF
  63 0004 4FF07FF5 	        DSB
  64 0008 1EFF2FE1 	        BX      LR
  65              	
  66              	
  67              	    .type   CPU_IntEn, %function
  68              	CPU_IntEn:
  69 000c 4FF07FF5 	        DSB
  70 0010 C00008F1 	        CPSIE   IF
  71 0014 1EFF2FE1 	        BX      LR
  72              	
  73              	
  74              	@**************************************************************************************************
  75              	@                                      CRITICAL SECTION FUNCTIONS
  76              	@
  77              	@ Description : Disable/Enable interrupts by preserving the state of interrupts.  Generally speakin
  78              	@               state of the interrupt disable flag is stored in the local variable 'cpu_sr' & inte
  79              	@               are then disabled ('cpu_sr' is allocated in all functions that need to disable inte
  80              	@               The previous interrupt state is restored by copying 'cpu_sr' into the CPU's status 
  81              	@
  82              	@ Prototypes  : CPU_SR  CPU_SR_Save   (void)@
  83              	@               void    CPU_SR_Restore(CPU_SR  cpu_sr)@
  84              	@
  85              	@ Note(s)     : (1) These functions are used in general like this :
  86              	@
  87              	@                       void  Task (void  *p_arg)
  88              	@                       {
  89              	@                           CPU_SR_ALLOC()@                     /* Allocate storage for CPU status 
  90              	@                               :
  91              	@                               :
  92              	@                           CPU_CRITICAL_ENTER()@               /* cpu_sr = CPU_SR_Save()@         
  93              	@                               :
  94              	@                               :
  95              	@                           CPU_CRITICAL_EXIT()@                /* CPU_SR_Restore(cpu_sr)@         
  96              	@                               :
  97              	@                       }
  98              	@**************************************************************************************************
  99              	
 100              	    .type   CPU_SR_Save, %function
 101              	CPU_SR_Save:
 102 0018 00000FE1 	        MRS     R0, CPSR
 103 001c C0000CF1 	        CPSID   IF                                              @ Set IRQ & FIQ bits in CPSR to DIS
 104 0020 4FF07FF5 	        DSB
 105 0024 1EFF2FE1 	        BX      LR                                              @ DISABLED, return the original CPS
 106              	
 107              	    .type   CPU_SR_Restore, %function
 108              	CPU_SR_Restore:
 109 0028 4FF07FF5 	        DSB
 110 002c 00F021E1 	        MSR     CPSR_c, R0
ARM GAS  /tmp/ccwZrbJW.s 			page 3


 111 0030 1EFF2FE1 	        BX      LR
 112              	
 113              	
 114              	@**************************************************************************************************
 115              	@                                         WAIT FOR INTERRUPT
 116              	@
 117              	@ Description : Enters sleep state, which will be exited when an interrupt is received.
 118              	@
 119              	@ Prototypes  : void  CPU_WaitForInt (void)
 120              	@
 121              	@ Argument(s) : none.
 122              	@**************************************************************************************************
 123              	
 124              	    .type   CPU_WaitForInt, %function
 125              	CPU_WaitForInt:
 126 0034 4FF07FF5 	        DSB
 127 0038 03F020E3 	        WFI                                     @ Wait for interrupt
 128 003c 1EFF2FE1 	        BX      LR
 129              	
 130              	
 131              	
 132              	@**************************************************************************************************
 133              	@                                         WAIT FOR EXCEPTION
 134              	@
 135              	@ Description : Enters sleep state, which will be exited when an exception is received.
 136              	@
 137              	@ Prototypes  : void  CPU_WaitForExcept (void)
 138              	@
 139              	@ Argument(s) : none.
 140              	@**************************************************************************************************
 141              	
 142              	    .type   CPU_WaitForEvent, %function
 143              	CPU_WaitForEvent:
 144 0040 4FF07FF5 	        DSB
 145 0044 02F020E3 	        WFE                                     @ Wait for exception
 146 0048 1EFF2FE1 	        BX      LR
 147              	
 148              	
 149              	@**************************************************************************************************
 150              	@                                         CPU_CntLeadZeros()
 151              	@                                        COUNT LEADING ZEROS
 152              	@
 153              	@ Description : Counts the number of contiguous, most-significant, leading zero bits before the fir
 154              	@               binary one bit in a data value.
 155              	@
 156              	@ Prototype   : CPU_DATA  CPU_CntLeadZeros(CPU_DATA  val);
 157              	@
 158              	@ Argument(s) : val         Data value to count leading zero bits.
 159              	@
 160              	@ Return(s)   : Number of contiguous, most-significant, leading zero bits in 'val'.
 161              	@
 162              	@ Note(s)     : (1) If the argument is zero, the value 32 is returned.
 163              	@
 164              	@               (2) MUST be implemented in cpu_a.asm if and only if CPU_CFG_LEAD_ZEROS_ASM_PRESENT 
 165              	@                   #define'd in 'cpu_cfg.h' or 'cpu.h'.
 166              	@**************************************************************************************************
 167              	
ARM GAS  /tmp/ccwZrbJW.s 			page 4


 168              	    .type   CPU_CntLeadZeros, %function
 169              	CPU_CntLeadZeros:
 170 004c 100F6FE1 	        CLZ     R0, R0                                          @ Count leading zeros
 171 0050 1EFF2FE1 	        BX      LR
 172              	
 173              	
 174              	@**************************************************************************************************
 175              	@                                         CPU_CntTrailZeros()
 176              	@                                        COUNT TRAILING ZEROS
 177              	@
 178              	@ Description : Counts the number of contiguous, least-significant, trailing zero bits before the
 179              	@                   first binary one bit in a data value.
 180              	@
 181              	@ Prototype   : CPU_DATA  CPU_CntTrailZeros(CPU_DATA  val);
 182              	@
 183              	@ Argument(s) : val         Data value to count trailing zero bits.
 184              	@
 185              	@ Return(s)   : Number of contiguous, least-significant, trailing zero bits in 'val'.
 186              	@
 187              	@ Note(s)     : (1) (a) Supports 32-bit data value size as configured by 'CPU_DATA' (see 'cpu.h
 188              	@                       CPU WORD CONFIGURATION  Note #1').
 189              	@
 190              	@                   (b) For 32-bit values :
 191              	@
 192              	@                             b31  b30  b29  b28  b27  ...  b02  b01  b00    # Trailing Zeros
 193              	@                             ---  ---  ---  ---  ---       ---  ---  ---    ----------------
 194              	@                              x    x    x    x    x         x    x    1            0
 195              	@                              x    x    x    x    x         x    1    0            1
 196              	@                              x    x    x    x    x         1    0    0            2
 197              	@                              :    :    :    :    :         :    :    :            :
 198              	@                              :    :    :    :    :         :    :    :            :
 199              	@                              x    x    x    x    1         0    0    0           27
 200              	@                              x    x    x    1    0         0    0    0           28
 201              	@                              x    x    1    0    0         0    0    0           29
 202              	@                              x    1    0    0    0         0    0    0           30
 203              	@                              1    0    0    0    0         0    0    0           31
 204              	@                              0    0    0    0    0         0    0    0           32
 205              	@
 206              	@
 207              	@               (2) MUST be defined in 'cpu_a.asm' (or 'cpu_c.c') if CPU_CFG_TRAIL_ZEROS_ASM_PRESEN
 208              	@                   #define'd in 'cpu_cfg.h' or 'cpu.h'.
 209              	@**************************************************************************************************
 210              	
 211              	    .type   CPU_CntTrailZeros, %function
 212              	CPU_CntTrailZeros:
 213 0054 300FFFE6 	        RBIT    R0, R0                          @ Reverse bits
 214 0058 100F6FE1 	        CLZ     R0, R0                          @ Count trailing zeros
 215 005c 1EFF2FE1 	        BX      LR
 216              	
 217              	
 218              	@**************************************************************************************************
 219              	@                                     CPU ASSEMBLY PORT FILE END
 220              	@**************************************************************************************************
 221              	
 222              	    .end
