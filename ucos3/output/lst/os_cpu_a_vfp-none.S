ARM GAS  /tmp/ccWdMeDb.s 			page 1


   1              	# 1 "Micrium/Software/uCOS-III/Source/../Ports/ARM-Cortex-A/ARMv7-A/GNU/os_cpu_a_vfp-none.S"
   1              	@**************************************************************************************************
   1              	...
   0              	
   0              	
   2              	@                                              uC/OS-III
   3              	@                                        The Real-Time Kernel
   4              	@
   5              	@                    Copyright 2009-2020 Silicon Laboratories Inc. www.silabs.com
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
  17              	@                                             ARMv7-A Port
  18              	@
  19              	@ File      : os_cpu_a_vfp-none.S
  20              	@ Version   : V3.08.00
  21              	@**************************************************************************************************
  22              	@ For       : ARM7 or ARM9
  23              	@ Mode      : ARM  or Thumb
  24              	@ Toolchain : GNU
  25              	@**************************************************************************************************
  26              	@ Note(s) : (1) See Note #2 of os_cpu.h for important informations about this file.
  27              	@**************************************************************************************************
  28              	
  29              	@**************************************************************************************************
  30              	@                                            FUNCTIONS
  31              	@**************************************************************************************************
  32              	                                                                @ .external references.
  33              	    .extern  OSRunning
  34              	    .extern  OSPrioCur
  35              	    .extern  OSPrioHighRdy
  36              	    .extern  OSTCBCurPtr
  37              	    .extern  OSTCBHighRdyPtr
  38              	    .extern  OSIntNestingCtr
  39              	    .extern  OSIntExit
  40              	    .extern  OSTaskSwHook
  41              	    .extern  OS_CPU_ExceptHndlr                                 @ Chip Support/BSP specific excepti
  42              	
  43              	    .extern  OS_CPU_ExceptStkBase
  44              	
  45              	                                                                @ Functions declared in this file.
  46              	    .global  OSStartHighRdy
  47              	    .global  OSCtxSw
  48              	    .global  OSIntCtxSw
  49              	
  50              	                                                                @ Functions related to exception ha
  51              	    .global  OS_CPU_ARM_ExceptUndefInstrHndlr
  52              	    .global  OS_CPU_ARM_ExceptSwiHndlr
  53              	    .global  OS_CPU_ARM_ExceptPrefetchAbortHndlr
ARM GAS  /tmp/ccWdMeDb.s 			page 2


  54              	    .global  OS_CPU_ARM_ExceptDataAbortHndlr
  55              	    .global  OS_CPU_ARM_ExceptIrqHndlr
  56              	    .global  OS_CPU_ARM_ExceptFiqHndlr
  57              	
  58              	    .global  OS_CPU_ARM_DRegCntGet
  59              	
  60              	
  61              	@**************************************************************************************************
  62              	@                                               EQUATES
  63              	@**************************************************************************************************
  64              	
  65              	    .equ     OS_CPU_ARM_CONTROL_INT_DIS,        0xC0                    @ Disable both FIQ and IRQ.
  66              	    .equ     OS_CPU_ARM_CONTROL_FIQ_DIS,        0x40                    @ Disable FIQ.
  67              	    .equ     OS_CPU_ARM_CONTROL_IRQ_DIS,        0x80                    @ Disable IRQ.
  68              	    .equ     OS_CPU_ARM_CONTROL_THUMB,          0x20                    @ Set THUMB mode.
  69              	    .equ     OS_CPU_ARM_CONTROL_ARM,            0x00                    @ Set ARM mode.
  70              	
  71              	    .equ     OS_CPU_ARM_MODE_MASK,              0x1F
  72              	    .equ     OS_CPU_ARM_MODE_USR,               0x10
  73              	    .equ     OS_CPU_ARM_MODE_FIQ,               0x11
  74              	    .equ     OS_CPU_ARM_MODE_IRQ,               0x12
  75              	    .equ     OS_CPU_ARM_MODE_SVC,               0x13
  76              	    .equ     OS_CPU_ARM_MODE_ABT,               0x17
  77              	    .equ     OS_CPU_ARM_MODE_UND,               0x1B
  78              	    .equ     OS_CPU_ARM_MODE_SYS,               0x1F
  79              	
  80              	    .equ     OS_CPU_ARM_EXCEPT_RESET,           0x00
  81              	    .equ     OS_CPU_ARM_EXCEPT_UNDEF_INSTR,     0x01
  82              	    .equ     OS_CPU_ARM_EXCEPT_SWI,             0x02
  83              	    .equ     OS_CPU_ARM_EXCEPT_PREFETCH_ABORT,  0x03
  84              	    .equ     OS_CPU_ARM_EXCEPT_DATA_ABORT,      0x04
  85              	    .equ     OS_CPU_ARM_EXCEPT_ADDR_ABORT,      0x05
  86              	    .equ     OS_CPU_ARM_EXCEPT_IRQ,             0x06
  87              	    .equ     OS_CPU_ARM_EXCEPT_FIQ,             0x07
  88              	
  89              	    .equ     OS_CPU_ARM_FPEXC_EN,               0x40000000               @VFP enable bit.
  90              	
  91              	
  92              	@**************************************************************************************************
  93              	@                                     CODE GENERATION DIRECTIVES
  94              	@**************************************************************************************************
  95              	
  96              	    .code 32
  97              	
  98              	
  99              	@**************************************************************************************************
 100              	@                                         START MULTITASKING
 101              	@                                      void OSStartHighRdy(void)
 102              	@
 103              	@ Note(s) : 1) OSStartHighRdy() MUST:
 104              	@              a) Call OSTaskSwHook() then,
 105              	@              b) Set OSRunning to OS_STATE_OS_RUNNING,
 106              	@              c) Switch to the highest priority task.
 107              	@**************************************************************************************************
 108              	
 109              	    .type   OSStartHighRdy, %function
 110              	OSStartHighRdy:
ARM GAS  /tmp/ccWdMeDb.s 			page 3


 111              	                                                                @ Change to SVC mode.
 112 0000 D3F021E3 	    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SVC)
 113 0004 1FF07FF5 	    CLREX                                                       @ Clear exclusive monitor.
 114              	
 115 0008 FEFFFFEB 	    BL      OSTaskSwHook                                        @ OSTaskSwHook();
 116              	
 117              	                                                                @ SWITCH TO HIGHEST PRIORITY TASK:
 118 000c 000000E3 	    MOVW    R0, #:lower16:OSTCBHighRdyPtr                       @    Get highest priority task TCB 
 119 0010 000040E3 	    MOVT    R0, #:upper16:OSTCBHighRdyPtr
 120 0014 000090E5 	    LDR     R0, [R0]                                            @    Get stack pointer,
 121 0018 00D090E5 	    LDR     SP, [R0]                                            @    Switch to the new stack,
 122              	
 123 001c 04009DE4 	    LDR     R0, [SP], #4                                        @    Pop new task's CPSR,
 124 0020 00F06FE1 	    MSR     SPSR_cxsf, R0
 125              	
 126 0024 FFDFFDE8 	    LDMFD   SP!, {R0-R12, LR, PC}^                              @    Pop new task's context.
 127              	
 128              	
 129              	@**************************************************************************************************
 130              	@                       PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
 131              	@
 132              	@ Note(s) : 1) OSCtxSw() is called in SVC mode with BOTH FIQ and IRQ interrupts DISABLED.
 133              	@
 134              	@           2) The pseudo-code for OSCtxSw() is:
 135              	@              a) Save the current task's context onto the current task's stack,
 136              	@              b) OSTCBCurPtr->StkPtr = SP;
 137              	@              c) OSTaskSwHook();
 138              	@              d) OSPrioCur           = OSPrioHighRdy;
 139              	@              e) OSTCBCurPtr         = OSTCBHighRdyPtr;
 140              	@              f) SP                  = OSTCBHighRdyPtr->StkPtr;
 141              	@              g) Restore the new task's context from the new task's stack,
 142              	@              h) Return to new task's code.
 143              	@
 144              	@           3) Upon entry:
 145              	@              OSTCBCurPtr      points to the OS_TCB of the task to suspend,
 146              	@              OSTCBHighRdyPtr  points to the OS_TCB of the task to resume.
 147              	@**************************************************************************************************
 148              	
 149              	    .type   OSCtxSw, %function
 150              	OSCtxSw:
 151              	                                                                @ SAVE CURRENT TASK'S CONTEXT:
 152 0028 00402DE9 	    STMFD   SP!, {LR}                                           @     Push return address,
 153 002c 00402DE9 	    STMFD   SP!, {LR}
 154 0030 FF1F2DE9 	    STMFD   SP!, {R0-R12}                                       @     Push registers,
 155 0034 00000FE1 	    MRS     R0, CPSR                                            @     Push current CPSR,
 156 0038 01001EE3 	    TST     LR, #1                                              @     See if called from Thumb mode
 157 003c 20008013 	    ORRNE   R0, R0, #OS_CPU_ARM_CONTROL_THUMB                   @     If yes, set the T-bit.
 158 0040 01002DE9 	    STMFD   SP!, {R0}
 159              	
 160 0044 1FF07FF5 	    CLREX                                                       @ Clear exclusive monitor.
 161              	
 162 0048 000000E3 	    MOVW    R0, #:lower16:OSTCBCurPtr                           @ OSTCBCurPtr->StkPtr = SP;
 163 004c 000040E3 	    MOVT    R0, #:upper16:OSTCBCurPtr
 164 0050 001090E5 	    LDR     R1, [R0]
 165 0054 00D081E5 	    STR     SP, [R1]
 166              	
 167 0058 FEFFFFEB 	    BL      OSTaskSwHook                                        @ OSTaskSwHook();
ARM GAS  /tmp/ccWdMeDb.s 			page 4


 168              	
 169 005c 000000E3 	    MOVW    R0, #:lower16:OSPrioCur                             @ OSPrioCur   = OSPrioHighRdy;
 170 0060 000040E3 	    MOVT    R0, #:upper16:OSPrioCur
 171 0064 001000E3 	    MOVW    R1, #:lower16:OSPrioHighRdy
 172 0068 001040E3 	    MOVT    R1, #:upper16:OSPrioHighRdy
 173 006c 0020D1E5 	    LDRB    R2, [R1]
 174 0070 0020C0E5 	    STRB    R2, [R0]
 175              	
 176 0074 000000E3 	    MOVW    R0, #:lower16:OSTCBCurPtr                           @ OSTCBCurPtr = OSTCBHighRdyPtr;
 177 0078 000040E3 	    MOVT    R0, #:upper16:OSTCBCurPtr
 178 007c 001000E3 	    MOVW    R1, #:lower16:OSTCBHighRdyPtr
 179 0080 001040E3 	    MOVT    R1, #:upper16:OSTCBHighRdyPtr
 180              	
 181 0084 002091E5 	    LDR     R2, [R1]
 182 0088 002080E5 	    STR     R2, [R0]
 183              	
 184 008c 00D092E5 	    LDR     SP, [R2]                                            @ SP = OSTCBHighRdyPtr->OSTCBStkPtr
 185              	
 186              	                                                                @ RESTORE NEW TASK'S CONTEXT:
 187 0090 0100BDE8 	    LDMFD   SP!, {R0}                                           @    Pop new task's CPSR,
 188 0094 00F06FE1 	    MSR     SPSR_cxsf, R0
 189              	
 190 0098 FFDFFDE8 	    LDMFD   SP!, {R0-R12, LR, PC}^                              @    Pop new task's context.
 191              	
 192              	
 193              	@**************************************************************************************************
 194              	@                   PERFORM A CONTEXT SWITCH (From interrupt level) - OSIntCtxSw()
 195              	@
 196              	@ Note(s) : 1) OSIntCtxSw() is called in SVC mode with BOTH FIQ and IRQ interrupts DISABLED.
 197              	@
 198              	@           2) The pseudo-code for OSCtxSw() is:
 199              	@              a) OSTaskSwHook();
 200              	@              b) OSPrioCur   = OSPrioHighRdy;
 201              	@              c) OSTCBCurPtr = OSTCBHighRdyPtr;
 202              	@              d) SP          = OSTCBHighRdyPtr->OSTCBStkPtr;
 203              	@              e) Restore the new task's context from the new task's stack,
 204              	@              f) Return to new task's code.
 205              	@
 206              	@           3) Upon entry:
 207              	@              OSTCBCurPtr      points to the OS_TCB of the task to suspend,
 208              	@              OSTCBHighRdyPtr  points to the OS_TCB of the task to resume.
 209              	@**************************************************************************************************
 210              	
 211              	    .type   OSIntCtxSw, %function
 212              	OSIntCtxSw:
 213 009c FEFFFFEB 	    BL      OSTaskSwHook                                        @ OSTaskSwHook();
 214              	
 215 00a0 000000E3 	    MOVW    R0, #:lower16:OSPrioCur                             @ OSPrioCur = OSPrioHighRdy;
 216 00a4 000040E3 	    MOVT    R0, #:upper16:OSPrioCur
 217 00a8 001000E3 	    MOVW    R1, #:lower16:OSPrioHighRdy
 218 00ac 001040E3 	    MOVT    R1, #:upper16:OSPrioHighRdy
 219 00b0 0020D1E5 	    LDRB    R2, [R1]
 220 00b4 0020C0E5 	    STRB    R2, [R0]
 221              	
 222 00b8 000000E3 	    MOVW    R0, #:lower16:OSTCBCurPtr                           @ OSTCBCurPtr = OSTCBHighRdyPtr;
 223 00bc 000040E3 	    MOVT    R0, #:upper16:OSTCBCurPtr
 224 00c0 001000E3 	    MOVW    R1, #:lower16:OSTCBHighRdyPtr
ARM GAS  /tmp/ccWdMeDb.s 			page 5


 225 00c4 001040E3 	    MOVT    R1, #:upper16:OSTCBHighRdyPtr
 226 00c8 002091E5 	    LDR     R2, [R1]
 227 00cc 002080E5 	    STR     R2, [R0]
 228              	
 229 00d0 00D092E5 	    LDR     SP, [R2]                                            @ SP = OSTCBHighRdyPtr->OSTCBStkPtr
 230              	
 231              	                                                                @ RESTORE NEW TASK'S CONTEXT:
 232 00d4 0100BDE8 	    LDMFD   SP!, {R0}                                           @    Pop new task's CPSR,
 233 00d8 00F06FE1 	    MSR     SPSR_cxsf, R0
 234              	
 235 00dc FFDFFDE8 	    LDMFD   SP!, {R0-R12, LR, PC}^                              @    Pop new task's context.
 236              	
 237              	
 238              	@**************************************************************************************************
 239              	@                               UNDEFINED INSTRUCTION EXCEPTION HANDLER
 240              	@
 241              	@ Register Usage:  R0     Exception Type
 242              	@                  R1
 243              	@                  R2     Return PC
 244              	@**************************************************************************************************
 245              	
 246              	OS_CPU_ARM_ExceptUndefInstrHndlr:
 247              	                                                                @ LR offset to return from this exc
 248 00e0 0F002DE9 	    STMFD   SP!, {R0-R3}                                        @ Push working registers.
 249 00e4 0E20A0E1 	    MOV     R2, LR                                              @ Save link register.
 250 00e8 0100A0E3 	    MOV     R0, #OS_CPU_ARM_EXCEPT_UNDEF_INSTR                  @ Set exception ID to OS_CPU_ARM_EX
 251 00ec 1C0000EA 	    B            OS_CPU_ARM_ExceptHndlr                         @ Branch to global exception handle
 252              	
 253              	
 254              	@**************************************************************************************************
 255              	@                                SOFTWARE INTERRUPT EXCEPTION HANDLER
 256              	@
 257              	@ Register Usage:  R0     Exception Type
 258              	@                  R1
 259              	@                  R2     Return PC
 260              	@**************************************************************************************************
 261              	
 262              	OS_CPU_ARM_ExceptSwiHndlr:
 263              	                                                                @ LR offset to return from this exc
 264 00f0 0F002DE9 	    STMFD   SP!, {R0-R3}                                        @ Push working registers.
 265 00f4 0E20A0E1 	    MOV     R2, LR                                              @ Save link register.
 266 00f8 0200A0E3 	    MOV     R0, #OS_CPU_ARM_EXCEPT_SWI                          @ Set exception ID to OS_CPU_ARM_EX
 267 00fc 180000EA 	    B            OS_CPU_ARM_ExceptHndlr                         @ Branch to global exception handle
 268              	
 269              	
 270              	@**************************************************************************************************
 271              	@                                  PREFETCH ABORT EXCEPTION HANDLER
 272              	@
 273              	@ Register Usage:  R0     Exception Type
 274              	@                  R1
 275              	@                  R2     Return PC
 276              	@**************************************************************************************************
 277              	
 278              	OS_CPU_ARM_ExceptPrefetchAbortHndlr:
 279 0100 04E04EE2 	    SUB     LR, LR, #4                                          @ LR offset to return from this exc
 280 0104 0F002DE9 	    STMFD   SP!, {R0-R3}                                        @ Push working registers.
 281 0108 0E20A0E1 	    MOV     R2, LR                                              @ Save link register.
ARM GAS  /tmp/ccWdMeDb.s 			page 6


 282 010c 0300A0E3 	    MOV     R0, #OS_CPU_ARM_EXCEPT_PREFETCH_ABORT               @ Set exception ID to OS_CPU_ARM_EX
 283 0110 130000EA 	    B            OS_CPU_ARM_ExceptHndlr                         @ Branch to global exception handle
 284              	
 285              	
 286              	@**************************************************************************************************
 287              	@                                    DATA ABORT EXCEPTION HANDLER
 288              	@
 289              	@ Register Usage:  R0     Exception Type
 290              	@                  R1
 291              	@                  R2     Return PC
 292              	@**************************************************************************************************
 293              	
 294              	OS_CPU_ARM_ExceptDataAbortHndlr:
 295 0114 08E04EE2 	    SUB     LR, LR, #8                                          @ LR offset to return from this exc
 296 0118 0F002DE9 	    STMFD   SP!, {R0-R3}                                        @ Push working registers.
 297 011c 0E20A0E1 	    MOV     R2, LR                                              @ Save link register.
 298 0120 0400A0E3 	    MOV     R0, #OS_CPU_ARM_EXCEPT_DATA_ABORT                   @ Set exception ID to OS_CPU_ARM_EX
 299 0124 0E0000EA 	    B            OS_CPU_ARM_ExceptHndlr                         @ Branch to global exception handle
 300              	
 301              	
 302              	@**************************************************************************************************
 303              	@                                   ADDRESS ABORT EXCEPTION HANDLER
 304              	@
 305              	@ Register Usage:  R0     Exception Type
 306              	@                  R1
 307              	@                  R2     Return PC
 308              	@**************************************************************************************************
 309              	
 310              	OS_CPU_ARM_ExceptAddrAbortHndlr:
 311 0128 08E04EE2 	    SUB     LR, LR, #8                                          @ LR offset to return from this exc
 312 012c 0F002DE9 	    STMFD   SP!, {R0-R3}                                        @ Push working registers.
 313 0130 0E20A0E1 	    MOV     R2, LR                                              @ Save link register.
 314 0134 0500A0E3 	    MOV     R0, #OS_CPU_ARM_EXCEPT_ADDR_ABORT                   @ Set exception ID to OS_CPU_ARM_EX
 315 0138 090000EA 	    B            OS_CPU_ARM_ExceptHndlr                         @ Branch to global exception handle
 316              	
 317              	
 318              	@**************************************************************************************************
 319              	@                                 INTERRUPT REQUEST EXCEPTION HANDLER
 320              	@
 321              	@ Register Usage:  R0     Exception Type
 322              	@                  R1
 323              	@                  R2     Return PC
 324              	@**************************************************************************************************
 325              	
 326              	OS_CPU_ARM_ExceptIrqHndlr:
 327 013c 04E04EE2 	    SUB     LR, LR, #4                                          @ LR offset to return from this exc
 328 0140 0F002DE9 	    STMFD   SP!, {R0-R3}                                        @ Push working registers.
 329 0144 0E20A0E1 	    MOV     R2, LR                                              @ Save link register.
 330 0148 0600A0E3 	    MOV     R0, #OS_CPU_ARM_EXCEPT_IRQ                          @ Set exception ID to OS_CPU_ARM_EX
 331 014c 040000EA 	    B            OS_CPU_ARM_ExceptHndlr                         @ Branch to global exception handle
 332              	
 333              	
 334              	@**************************************************************************************************
 335              	@                              FAST INTERRUPT REQUEST EXCEPTION HANDLER
 336              	@
 337              	@ Register Usage:  R0     Exception Type
 338              	@                  R1
ARM GAS  /tmp/ccWdMeDb.s 			page 7


 339              	@                  R2     Return PC
 340              	@**************************************************************************************************
 341              	
 342              	OS_CPU_ARM_ExceptFiqHndlr:
 343 0150 04E04EE2 	    SUB     LR, LR, #4                                          @ LR offset to return from this exc
 344 0154 0F002DE9 	    STMFD   SP!, {R0-R3}                                        @ Push working registers.
 345 0158 0E20A0E1 	    MOV     R2, LR                                              @ Save link register.
 346 015c 0700A0E3 	    MOV     R0, #OS_CPU_ARM_EXCEPT_FIQ                          @ Set exception ID to OS_CPU_ARM_EX
 347 0160 FFFFFFEA 	    B            OS_CPU_ARM_ExceptHndlr                         @ Branch to global exception handle
 348              	
 349              	
 350              	@**************************************************************************************************
 351              	@                                      GLOBAL EXCEPTION HANDLER
 352              	@
 353              	@ Register Usage:  R0     Exception Type
 354              	@                  R1     Exception's SPSR
 355              	@                  R2     Return PC
 356              	@                  R3     Exception's SP
 357              	@
 358              	@ Note(s)       : 1) An exception can occur in three different circumstances; in each of these, the
 359              	@                    SVC stack pointer will point to a different entity :
 360              	@
 361              	@                    a) CONDITION: An exception occurs before the OS has been fully initialized.
 362              	@                       SVC STACK: Should point to a stack initialized by the application's startup
 363              	@                       STK USAGE: Interrupted context -- SVC stack.
 364              	@                                  Exception           -- SVC stack.
 365              	@                                  Nested exceptions   -- SVC stack.
 366              	@
 367              	@                    b) CONDITION: An exception interrupts a task.
 368              	@                       SVC STACK: Should point to task stack.
 369              	@                       STK USAGE: Interrupted context -- Task stack.
 370              	@                                  Exception           -- Exception stack 'OS_CPU_ExceptStk[]'.
 371              	@                                  Nested exceptions   -- Exception stack 'OS_CPU_ExceptStk[]'.
 372              	@
 373              	@                    c) CONDITION: An exception interrupts another exception.
 374              	@                       SVC STACK: Should point to location in exception stack, 'OS_CPU_ExceptStk[]
 375              	@                       STK USAGE: Interrupted context -- Exception stack 'OS_CPU_ExceptStk[]'.
 376              	@                                  Exception           -- Exception stack 'OS_CPU_ExceptStk[]'.
 377              	@                                  Nested exceptions   -- Exception stack 'OS_CPU_ExceptStk[]'.
 378              	@**************************************************************************************************
 379              	
 380              	OS_CPU_ARM_ExceptHndlr:
 381 0164 00104FE1 	    MRS     R1, SPSR                                            @ Save CPSR (i.e. exception's SPSR)
 382 0168 0D30A0E1 	    MOV     R3, SP                                              @ Save exception's stack pointer.
 383              	
 384              	                                                                @ Adjust exception stack pointer.  
 385              	                                                                @ exception stack is not used when 
 386 016c 10D08DE2 	    ADD     SP, SP, #(4 * 4)
 387              	
 388              	                                                                @ Change to SVC mode & disable inte
 389 0170 D3F021E3 	    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SVC)
 390 0174 1FF07FF5 	    CLREX                                                       @ Clear exclusive monitor.
 391              	
 392 0178 04002DE9 	    STMFD   SP!, {R2}                                           @   Push task's PC,
 393 017c 00402DE9 	    STMFD   SP!, {LR}                                           @   Push task's LR,
 394 0180 F01F2DE9 	    STMFD   SP!, {R4-R12}                                       @   Push task's R12-R4,
 395 0184 E001B3E8 	    LDMFD   R3!, {R5-R8}                                        @   Move task's R3-R0 from exceptio
ARM GAS  /tmp/ccWdMeDb.s 			page 8


 396 0188 E0012DE9 	    STMFD   SP!, {R5-R8}
 397 018c 02002DE9 	    STMFD   SP!, {R1}                                           @   Push task's CPSR (i.e. exceptio
 398              	
 399 0190 003000E3 	    MOVW    R3, #:lower16:OSRunning                             @ if (OSRunning == 1)
 400 0194 003040E3 	    MOVT    R3, #:upper16:OSRunning
 401 0198 0040D3E5 	    LDRB    R4, [R3]
 402 019c 010054E3 	    CMP     R4, #1
 403 01a0 2700001A 	    BNE     OS_CPU_ARM_ExceptHndlr_BreakNothing
 404              	
 405              	                                                                @ HANDLE NESTING COUNTER:
 406 01a4 003000E3 	    MOVW    R3, #:lower16:OSIntNestingCtr                       @   OSIntNestingCtr++;
 407 01a8 003040E3 	    MOVT    R3, #:upper16:OSIntNestingCtr
 408 01ac 0040D3E5 	    LDRB    R4, [R3]
 409 01b0 014084E2 	    ADD     R4, R4, #1
 410 01b4 0040C3E5 	    STRB    R4, [R3]
 411              	
 412 01b8 010054E3 	    CMP     R4, #1                                              @ if (OSIntNestingCtr == 1)
 413 01bc 1000001A 	    BNE     OS_CPU_ARM_ExceptHndlr_BreakExcept
 414              	
 415              	
 416              	@**************************************************************************************************
 417              	@                                 EXCEPTION HANDLER: TASK INTERRUPTED
 418              	@
 419              	@ Register Usage:  R0     Exception Type
 420              	@                  R1
 421              	@                  R2
 422              	@                  R3
 423              	@**************************************************************************************************
 424              	
 425              	OS_CPU_ARM_ExceptHndlr_BreakTask:
 426 01c0 003000E3 	    MOVW    R3, #:lower16:OSTCBCurPtr                           @ OSTCBCurPtr->StkPtr = SP;
 427 01c4 003040E3 	    MOVT    R3, #:upper16:OSTCBCurPtr
 428 01c8 004093E5 	    LDR     R4, [R3]
 429 01cc 00D084E5 	    STR     SP, [R4]
 430              	
 431              	
 432 01d0 003000E3 	    MOVW    R3, #:lower16:OS_CPU_ExceptStkBase                  @ Switch to exception stack.
 433 01d4 003040E3 	    MOVT    R3, #:upper16:OS_CPU_ExceptStkBase
 434 01d8 00D093E5 	    LDR     SP, [R3]
 435              	
 436              	                                                                @ EXECUTE EXCEPTION HANDLER:
 437 01dc FEFFFFEB 	    BL      OS_CPU_ExceptHndlr                                  @ OS_CPU_ExceptHndlr(except_type = 
 438              	
 439              	                                                                @ Change to SVC mode & disable inte
 440 01e0 D3F021E3 	    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SVC)
 441              	
 442              	                                                                @ Call OSIntExit().  This call MAY 
 443              	                                                                @ task with higher priority than th
 444              	                                                                @ found.
 445 01e4 FEFFFFEB 	    BL      OSIntExit
 446              	
 447 01e8 003000E3 	    MOVW    R3, #:lower16:OSTCBCurPtr                           @ SP = OSTCBCurPtr->StkPtr;
 448 01ec 003040E3 	    MOVT    R3, #:upper16:OSTCBCurPtr
 449 01f0 004093E5 	    LDR     R4, [R3]
 450 01f4 00D094E5 	    LDR     SP, [R4]
 451              	
 452              	                                                                @ RESTORE NEW TASK'S CONTEXT:
ARM GAS  /tmp/ccWdMeDb.s 			page 9


 453 01f8 0100BDE8 	    LDMFD   SP!, {R0}                                           @    Pop new task's CPSR,
 454 01fc 00F06FE1 	    MSR     SPSR_cxsf, R0
 455              	
 456 0200 FFDFFDE8 	    LDMFD   SP!, {R0-R12, LR, PC}^                              @    Pop new task's context.
 457              	
 458              	
 459              	@**************************************************************************************************
 460              	@                              EXCEPTION HANDLER: EXCEPTION INTERRUPTED
 461              	@
 462              	@ Register Usage:  R0     Exception Type
 463              	@                  R1
 464              	@                  R2
 465              	@                  R3
 466              	@**************************************************************************************************
 467              	
 468              	OS_CPU_ARM_ExceptHndlr_BreakExcept:
 469              	
 470 0204 0D10A0E1 	    MOV     R1, SP
 471 0208 041001E2 	    AND     R1, R1, #4
 472 020c 01D04DE0 	    SUB     SP, SP, R1
 473 0210 02402DE9 	    STMFD   SP!, {R1, LR}
 474              	                                                                @ EXECUTE EXCEPTION HANDLER:
 475 0214 FEFFFFEB 	    BL      OS_CPU_ExceptHndlr                                  @ OS_CPU_ExceptHndlr(except_type = 
 476              	
 477 0218 0240BDE8 	    LDMIA   SP!, {R1, LR}
 478 021c 01D08DE0 	    ADD     SP, SP, R1
 479              	
 480              	                                                                @ Change to SVC mode & disable inte
 481 0220 D3F021E3 	    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SVC)
 482              	
 483              	                                                                @ HANDLE NESTING COUNTER:
 484 0224 003000E3 	    MOVW    R3, #:lower16:OSIntNestingCtr                       @   OSIntNestingCtr--;
 485 0228 003040E3 	    MOVT    R3, #:upper16:OSIntNestingCtr
 486 022c 0040D3E5 	    LDRB    R4, [R3]
 487 0230 014044E2 	    SUB     R4, R4, #1
 488 0234 0040C3E5 	    STRB    R4, [R3]
 489              	
 490              	                                                                @ RESTORE OLD CONTEXT:
 491 0238 0100BDE8 	    LDMFD   SP!, {R0}                                           @    Pop old CPSR,
 492 023c 00F06FE1 	    MSR     SPSR_cxsf, R0
 493              	
 494 0240 FFDFFDE8 	    LDMFD   SP!, {R0-R12, LR, PC}^                              @   Pull working registers and retu
 495              	
 496              	
 497              	@**************************************************************************************************
 498              	@                              EXCEPTION HANDLER: 'NOTHING' INTERRUPTED
 499              	@
 500              	@ Register Usage:  R0     Exception Type
 501              	@                  R1
 502              	@                  R2
 503              	@                  R3
 504              	@**************************************************************************************************
 505              	
 506              	OS_CPU_ARM_ExceptHndlr_BreakNothing:
 507              	
 508 0244 0D10A0E1 	    MOV     R1, SP
 509 0248 041001E2 	    AND     R1, R1, #4
ARM GAS  /tmp/ccWdMeDb.s 			page 10


 510 024c 01D04DE0 	    SUB     SP, SP, R1
 511 0250 02402DE9 	    STMFD   SP!, {R1, LR}
 512              	
 513              	                                                                @ EXECUTE EXCEPTION HANDLER:
 514 0254 FEFFFFEB 	    BL      OS_CPU_ExceptHndlr                                  @ OS_CPU_ExceptHndlr(except_type = 
 515              	
 516 0258 0240BDE8 	    LDMIA   SP!, {R1, LR}
 517 025c 01D08DE0 	    ADD     SP, SP, R1
 518              	
 519              	                                                                @ Change to SVC mode & disable inte
 520 0260 D3F021E3 	    MSR     CPSR_c, #(OS_CPU_ARM_CONTROL_INT_DIS | OS_CPU_ARM_MODE_SVC)
 521              	
 522              	                                                                @ RESTORE OLD CONTEXT:
 523 0264 0100BDE8 	    LDMFD   SP!, {R0}                                           @   Pop old CPSR,
 524 0268 00F06FE1 	    MSR     SPSR_cxsf, R0
 525              	
 526 026c FFDFFDE8 	    LDMFD   SP!, {R0-R12, LR, PC}^                              @   Pull working registers and retu
 527              	
 528              	
 529              	@**************************************************************************************************
 530              	@                              VFP/NEON REGISTER COUNT
 531              	@
 532              	@ Register Usage:  R0     Double Register Count
 533              	@**************************************************************************************************
 534              	
 535              	    .type   OS_CPU_ARM_DRegCntGet, %function
 536              	OS_CPU_ARM_DRegCntGet:
 537 0270 0000A0E3 	    MOV     R0, #0
 538 0274 1EFF2FE1 	    BX      LR
