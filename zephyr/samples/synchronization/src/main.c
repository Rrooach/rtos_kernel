/* main.c - Hello World demo */

/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
 
#include <zephyr/sys/crc.h> 
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>
#include <string.h>

#define SYSCALLAPI
#define ALIGNED(N) __attribute__((aligned(N)))
// #define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define kMaxArgs 13
#define kCoverSize 256 << 10
#define kMaxCommands 1000
#define instr_eof -1
#define MAX_PROG_SIZE (1 << 15)
#define MAX_DATA_SIZE (1 << 15)

// define input args type
#define INT_TYPE 0
#define RES_TYPE_IN 1
#define RES_TYPE_OUT 2
#define PTR_TYPE 3
#define STRC_TYPE 4
#define DATA_TYPE 5
#define GROUP_TYPE 8

// define execution status
#define WAIT 0
#define READY 1
#define RUNNING 2
typedef unsigned long long __uint64_t;
typedef unsigned int __uint32_t;
typedef unsigned short __uint16_t;
typedef unsigned char __uint8_t; 

__uint8_t executor_status[2] = "wa";
__uint8_t misc[104] = "ferfer"; 
__uint64_t args_list[kMaxArgs+1];
__uint8_t corpus_buffer[MAX_PROG_SIZE] = "feryrferyrferyr";
__uint64_t data_buffer[MAX_DATA_SIZE] = {0};
__uint8_t clr_buffer[30];
__uint8_t clr_idx = 0;
__uint32_t ret_buffer[20];

typedef struct
{
    __uint64_t disabled;
    __uint64_t timeout;
    __uint64_t prog_timeout;
    __uint64_t ignore_return;
    __uint64_t breaks_returns;
} call_attrs_t;

typedef struct
{
    int fail_nth;
} call_props_t;

#define SYSCALLAPI
typedef intptr_t(SYSCALLAPI *syscall_t)(intptr_t, intptr_t, intptr_t, intptr_t,
                                        intptr_t, intptr_t, intptr_t, intptr_t,
                                        intptr_t, intptr_t, intptr_t, intptr_t,
                                        intptr_t);

typedef struct
{
    __uint64_t syscall_id;
    __uint64_t args_val[kMaxArgs];
    __uint64_t args_typ[kMaxArgs];
    __uint64_t ret;
} syscall;

typedef struct
{
    const char *name;
    int sys_nr;
    call_attrs_t attrs;
    syscall_t call;
} call_t;
 
typedef struct
{
    __uint32_t executed;
    __uint64_t val;
} res_t;
  
#include "executor_zephyr.h"
#include "syscalls.h"

void exec_print()
{
    printf(" Executor: ******************** begin exec ************************\n");
}

__uint64_t read_input(__uint64_t **input_posp, __uint32_t peek)
{
    __uint64_t *input_pos = *input_posp;  
    if ((__uint8_t *)input_pos >= corpus_buffer + MAX_PROG_SIZE)
        printf("input command overflows input pos=%p: [%p:%p)\n", input_pos,
                        corpus_buffer, corpus_buffer + MAX_PROG_SIZE);
    if (!peek)
        *input_posp = input_pos + 1;
    return *input_pos;
} 

void serialize_call()
{
    __uint64_t *input_pos = (__uint64_t *)corpus_buffer;

    // read syscall id
    syscall current_call;

    // init
    for (__uint32_t i = 0; i < kMaxArgs; ++i)
    {
        current_call.args_typ[i] = 0;
        current_call.args_val[i] = 0;
        args_list[i] = 0;
    }
    // Mem_Set(ret_buffer, 0xffffffff, sizeof(__uint32_t)*20);
    

    // current_call.syscall_id = read_input(&input_pos, 0);
    if (current_call.syscall_id == instr_eof)
    {
        printf("exit because of eof");
        return;
    }

    // printf("name = %s\n", syscalls[current_call.syscall_id].name); 

    // Abnormal syscall.
    if (current_call.syscall_id >= ARRAY_SIZE(syscalls))
        printf("call_num=%llu is invalid\n", current_call.syscall_id);

    // read val array
    __uint64_t val_len = read_input(&input_pos, 0); 
    printf("val_len = %lld\n", val_len); 
    for (__uint64_t args_idx = 0; args_idx < val_len; ++args_idx)
    {
        current_call.args_val[args_idx] = read_input(&input_pos, 0);
    }

    // read typ array
    __uint64_t typ_len = read_input(&input_pos, 0);
    printf("typ_len = %lld\n", typ_len);
    for (__uint64_t args_idx = 0; args_idx < typ_len; ++args_idx)
    {
        current_call.args_typ[args_idx] = read_input(&input_pos, 0);
    }

    // // return return type
    args_list[kMaxArgs] = read_input(&input_pos, 0);
    printf("\nreturn type = %lld\n", args_list[kMaxArgs]);

    // read buffer
    __uint64_t data_len = read_input(&input_pos, 0);
    for (__uint64_t args_idx = 0; args_idx < data_len; ++args_idx)
    {
        data_buffer[args_idx] = read_input(&input_pos, 0);
        // printf(("data index = %lld, data val = %lld\n", args_idx, data_buffer[args_idx]));
    }

    for (__uint64_t i = 0; i < val_len; ++i)
    {
        switch (current_call.args_typ[i])
        {
            __uint64_t off_set = 0, str_id = 0;
        case INT_TYPE: // 0 right
            args_list[i] = current_call.args_val[i];
            break;
        case RES_TYPE_IN: // 1
            off_set = current_call.args_val[i];
            if(ret_buffer[off_set] != 0xffffffff){
                args_list[i] = ret_buffer[current_call.args_val[i]];
                printf("idx = %lld was a res type; branch 2\n", i);
            }
            else 
                args_list[i] = current_call.args_val[i];
            break;
        case RES_TYPE_OUT: // 2
            // args_list[i] = current_call.args_val[i];
            off_set = current_call.args_val[i];
            if(ret_buffer[off_set] != 0xffffffff){
                args_list[i] = ret_buffer[current_call.args_val[i]];
                printf("idx = %lld was a res type; branch 1\n", i);
            }
            else 
                args_list[i] = current_call.args_val[i];
            break;
            break;
        case (INT_TYPE ^ PTR_TYPE): // 3
            off_set = current_call.args_val[i];
            args_list[i] = (__uint64_t)&data_buffer[off_set];
            break;
        case (DATA_TYPE ^ PTR_TYPE): // 6
            off_set = current_call.args_val[i];
            args_list[i] = (__uint64_t)&data_buffer[off_set];
            break;
        case (STRC_TYPE ^ PTR_TYPE): // 7
            str_id = current_call.args_val[i];
            args_list[i] = struct_set[str_id / MAX_ARRAY_LEN] + (str_id % MAX_ARRAY_LEN) * struct_size[str_id / MAX_ARRAY_LEN];
            clr_buffer[clr_idx++] = str_id;
            ret_buffer[str_id % MAX_ARRAY_LEN] = args_list[i];
            break;
        case (GROUP_TYPE ^ PTR_TYPE): // 11
            off_set = current_call.args_val[i];
            args_list[i] = (__uint64_t)&data_buffer[off_set];
            printf("in group: i = %lld, val = %lld\n", i, data_buffer[off_set]);
            break;
        default:
            printf("Not Validate Flags");
            break;
        }
    }

    for (__uint64_t i = 0; i < kMaxArgs; ++i)
    {
        printf("arg[%lld] = %lld\n", i, args_list[i]);
    }

    // const
    call_t *call = (call_t*)&syscalls[current_call.syscall_id];

    execute_syscall(call, args_list);
    // __uint64_t call_end = ;
    read_input(&input_pos, 0);

    exec_print();
    // TODO data write back
    // if (running > 0) {

    // }
}
/*
 * The hello world demo has two threads that utilize semaphores and sleeping
 * to take turns printing a greeting message at a controlled rate. The demo
 * shows both the static and dynamic approaches for spawning a thread; a real
 * world application would likely use the static approach for both threads.
 */

#define PIN_THREADS (IS_ENABLED(CONFIG_SMP)		  \
		     && IS_ENABLED(CONFIG_SCHED_CPU_MASK) \
		     && (CONFIG_MP_NUM_CPUS > 1))

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

/* delay between greetings (in ms) */
#define SLEEPTIME 500


/*
 * @param my_name      thread identification string
 * @param my_sem       thread's own semaphore
 * @param other_sem    other thread's semaphore
 */
void helloLoop(const char *my_name,
	       struct k_sem *my_sem, struct k_sem *other_sem)
{
	const char *tname;
	uint8_t cpu;
	struct k_thread *current_thread;

	while (1) {
		/* take my semaphore */
		k_sem_take(my_sem, K_FOREVER);

		current_thread = k_current_get();
		tname = k_thread_name_get(current_thread);
#if CONFIG_SMP
		cpu = arch_curr_cpu()->id;
#else
		cpu = 0;
#endif
		/* say "hello" */
		if (tname == NULL) {
			printk("%s: Hello World from cpu %d on %s!\n",
				my_name, cpu, CONFIG_BOARD);
		} else {
			printk("%s: Hello World from cpu %d on %s!\n",
				tname, cpu, CONFIG_BOARD);
		}

		/* wait a while, then let other thread have a turn */
		k_busy_wait(100000);
		k_msleep(SLEEPTIME);
		k_sem_give(other_sem);
	}
}

/* define semaphores */

K_SEM_DEFINE(threadA_sem, 1, 1);	/* starts off "available" */
K_SEM_DEFINE(threadB_sem, 0, 1);	/* starts off "not available" */


/* threadB is a dynamic thread that is spawned by threadA */

void threadB(void *dummy1, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	/* invoke routine to ping-pong hello messages with threadA */
	helloLoop(__func__, &threadB_sem, &threadA_sem);
}

K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;

K_THREAD_STACK_DEFINE(threadB_stack_area, STACKSIZE);
static struct k_thread threadB_data;

/* threadA is a static thread that is spawned automatically */

void threadA(void *dummy1, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	/* invoke routine to ping-pong hello messages with threadB */
	helloLoop(__func__, &threadA_sem, &threadB_sem);
}
 
void main(void)
{  
    for( ;; ) {
        
        // printf("loop...\n\n");
        misc[1] = '0'; 
		if (executor_status[0] == 'r' && executor_status[1] == 'e')
        { 
            printf("in the ready logic...\n\n");
            serialize_call();
            executor_status[0] = 'w', executor_status[1] = 'a';
        }
		else if (executor_status[0] == 'c' && executor_status[1] == 'l')
        {
            // reset kernel status 
            serialize_call();
            printf("in the reset logic...\n\n");
            // clear_struct();
            executor_status[0] = 'w', executor_status[1] = 'a';
            printf("reset logic has done...");
        }
 
	} 
    
}
