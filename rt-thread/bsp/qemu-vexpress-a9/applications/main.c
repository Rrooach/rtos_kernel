#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dfs.h>
#include <dfs_fs.h>
#include <unistd.h> 
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define MAX_DATA_SIZE (2 << 10)

#define SYSCALLAPI
#define ALIGNED(N) __attribute__((aligned(N)))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define kMaxArgs 13
#define kMaxCommands 1000
#define instr_eof -1
#define MAX_PROG_SIZE (1<<16)
#define MAX_ARRAY_LEN (1<<16)

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

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

typedef unsigned long long __uint64_t; 
typedef unsigned short __uint16_t;
typedef unsigned char __uint8_t;

// __uint8_t executor_status[0] = "w";
__uint8_t executor_status[2] = "wa";
__uint8_t misc[1024] = "0";
__uint64_t args_list[kMaxArgs+1];
__uint8_t corpus_buffer[MAX_PROG_SIZE] = "feryrferyrferyr";
__uint64_t data_buffer[MAX_DATA_SIZE] = {0};
__uint8_t clr_buffer[30];
__uint8_t clr_idx = 0;
__uint32_t ret_buffer[20];
 

/*
*********************************************************************************************************
*                                       LOCAL STRUCTURE DECLARE
*********************************************************************************************************
*/

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

static res_t results[kMaxCommands];
 
#include "executor_rtthread.h"
#include "syscalls.h"

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static __uint64_t read_input(__uint64_t **input_posp, __uint32_t peek);
static __uint64_t read_arg(__uint64_t **input_posp);
static __uint64_t read_result(__uint64_t **input_posp);
static __uint64_t swap(__uint64_t v, __uint64_t size, __uint64_t bf);
static void StartupTask(void *p_arg);

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION Definition
*********************************************************************************************************
*/

void exec_print()
{
    printf(" Executor: ******************** begin exec ************************\n");
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
    for (int i = 0; i < 20; ++i) {
        ret_buffer[i] = 0xffffffff;
    }
    // Mem_Set(ret_buffer, 0xffffffff, sizeof(__uint32_t)*20);
    

    current_call.syscall_id = read_input(&input_pos, 0); 
    if (current_call.syscall_id == instr_eof)
    {
        printf("exit because of eof, the syascall id is %lld\n", current_call.syscall_id);
        return;
    }

    printf("id = %lld, name = %s\n", current_call.syscall_id, syscalls[current_call.syscall_id].name);

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
  
    // return return type
    args_list[kMaxArgs] = read_input(&input_pos, 0);
    printf("\nreturn type = %lld\n", args_list[kMaxArgs]);

    // read buffer
    __uint64_t data_len = read_input(&input_pos, 0);
    for (__uint64_t args_idx = 0; args_idx < data_len; ++args_idx)
    {
        data_buffer[args_idx] = read_input(&input_pos, 0);
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
                printf("idx = %lld was a res type; branch 2\n", i);}
            else 
                args_list[i] = current_call.args_val[i];
            break;
        case RES_TYPE_OUT: // 2
            // args_list[i] = current_call.args_val[i];
            off_set = current_call.args_val[i];
            if(ret_buffer[off_set] != 0xffffffff){
                args_list[i] = ret_buffer[current_call.args_val[i]];
                printf("idx = %lld was a res type; branch 1\n", i);}
            else 
                args_list[i] = current_call.args_val[i];
            break;
            break;
        case (INT_TYPE ^ PTR_TYPE): // 3
            off_set = current_call.args_val[i];
            args_list[i] = &data_buffer[off_set];
            break;
        case (DATA_TYPE ^ PTR_TYPE): // 6
            off_set = current_call.args_val[i];
            args_list[i] = &data_buffer[off_set];
            break;
        case (STRC_TYPE ^ PTR_TYPE): // 7
            str_id = current_call.args_val[i];
            // args_list[i] = struct_set[str_id / MAX_ARRAY_LEN] + (str_id % MAX_ARRAY_LEN) * struct_size[str_id / MAX_ARRAY_LEN];
            clr_buffer[clr_idx++] = str_id;
            ret_buffer[str_id % MAX_ARRAY_LEN] = args_list[i];
            break;
        case (GROUP_TYPE ^ PTR_TYPE): // 11
            off_set = current_call.args_val[i];
            args_list[i] = &data_buffer[off_set];
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
    const call_t *call = &syscalls[current_call.syscall_id];

    execute_syscall(call, args_list);
    read_input(&input_pos, 0);
    exec_print();
    // TODO data write back
    // if (running > 0) {

    // }
}

__uint64_t read_result(__uint64_t **input_posp)
{
    __uint64_t idx = read_input(input_posp, 0);
    __uint64_t op_div = read_input(input_posp, 0);
    __uint64_t op_add = read_input(input_posp, 0);
    __uint64_t arg = read_input(input_posp, 0);
    if (idx >= kMaxCommands)
        printf("command refers to bad result, result=%lld\n", idx);
    if (results[idx].executed)
    {
        arg = results[idx].val;
        if (op_div != 0)
            arg = arg / op_div;
        arg += op_add;
    }
    return arg;
}

__uint8_t read_input_8(__uint8_t **input_posp, __uint32_t peek)
{
    __uint8_t *input_pos = *input_posp;
    if (!peek)
        *input_posp = input_pos + 1;
    return *input_pos;
}

__uint64_t read_input(__uint64_t **input_posp, __uint32_t peek)
{
    __uint64_t *input_pos = *input_posp;
    if ((__uint8_t *)input_pos >= corpus_buffer + MAX_PROG_SIZE)
        printf("input command overflows input, pos=%p: [%p:%p)\n", input_pos,
                        corpus_buffer, corpus_buffer + MAX_PROG_SIZE);
    if (!peek)
        *input_posp = input_pos + 1;
    return *input_pos;
}
 
void kernel_print()
{
    printf("\n\n******************** OS Started ************************\n\n");
} 
 
void clear_struct()
{
    for (__uint8_t i = 0, idx = 0; i < clr_idx; i++)
    {
        idx = clr_buffer[i];
        // Mem_Clr(struct_set[idx / MAX_ARRAY_LEN] + (idx % MAX_ARRAY_LEN) * 
                // struct_size[idx / MAX_ARRAY_LEN], struct_size[idx / MAX_ARRAY_LEN]);
    }
    // Mem_Clr(clr_buffer, sizeof(__uint8_t) * (clr_idx + 1));
    // clr_idx = 0;

    for (int i = 0; i < 20; ++i) {
        ret_buffer[i] = 0xffffffff;
    }
}
  
int main(void)
{ 
 
    while (1) {
        printf("Hello RISC-V!\n");
        if (executor_status[0] == 'r' && executor_status[1] == 'e')
        {

            exec_print();
            serialize_call();
            executor_status[0] = 'w', executor_status[1] = 'a';
            printf(("current status = %c%c\n\n", executor_status[0], executor_status[1]));
        }
        else if (executor_status[0] == 'c' && executor_status[1] == 'l')
        {
            // reset kernel status
            exec_print();
            serialize_call();
            printf(("in the reset logic...\n\n"));
            clear_struct();
            executor_status[0] = 'w', executor_status[1] = 'a';
            printf(("reset logic has done..."));
        }
    }
      

}
 