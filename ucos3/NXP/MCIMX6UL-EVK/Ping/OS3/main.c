
#include <cpu.h>
#include <cpu_core.h>
#include <lib_mem.h>
#include <lib_str.h>
#include <os.h>

#include "../app_cfg.h"
#include "os_app_hooks.h"

#if (APP_CFG_TCPIP_EN > 0u)
#include "../app_tcpip.h"
#include <Source/net.h>
#endif

#include <bsp_gpio.h>
#include <bsp_int.h>
#include <bsp_iomux.h>
#include <bsp_os.h>
#include <bsp_ser.h>
#include <bsp_sys.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define SYSCALLAPI
#define ALIGNED(N) __attribute__((aligned(N)))
#define INPUT_DATA_ALIGNMENT 64 << 10
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define kMaxArgs 13
#define kCoverSize 256 << 10
#define kMaxCommands 1000
#define instr_eof -1
#define MAX_PROG_SIZE (2 << 20)
#define MAX_DATA_SIZE (2 << 16)

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
typedef unsigned int __uint32_t;
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

ALIGNED(INPUT_DATA_ALIGNMENT)

static CPU_STK StartupTaskStk[APP_CFG_TASK_STARTUP_STK_SIZE];
static OS_TCB StartupTaskTCB;

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

#include "defs.h"
#include "executor_ucos.h"
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

void kernel_print()
{
    APP_TRACE_INFO((" Executor: ******************** OS end ************************\n"));
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
    Mem_Set(ret_buffer, 0xffffffff, sizeof(__uint32_t)*20);
    

    current_call.syscall_id = read_input(&input_pos, 0); 
    if (current_call.syscall_id == instr_eof)
    {
        APP_TRACE_INFO((" Executor: exit because of eof, the syascall id is %lld\n", current_call.syscall_id));
        return;
    }

    APP_TRACE_INFO((" Executor: name = %s\n", syscalls[current_call.syscall_id].name));

    // Abnormal syscall.
    if (current_call.syscall_id >= ARRAY_SIZE(syscalls))
        APP_TRACE_INFO((" Executor: x", "call_num=%llu is invalid\n", current_call.syscall_id));

    // read val array
    __uint64_t val_len = read_input(&input_pos, 0);
    APP_TRACE_INFO((" Executor: val_len = %lld\n", val_len));
    for (__uint64_t args_idx = 0; args_idx < val_len; ++args_idx)
    {
        current_call.args_val[args_idx] = read_input(&input_pos, 0);
    }

    // read typ array
    __uint64_t typ_len = read_input(&input_pos, 0);
    APP_TRACE_INFO((" Executor: typ_len = %lld\n", typ_len));
    for (__uint64_t args_idx = 0; args_idx < typ_len; ++args_idx)
    {
        current_call.args_typ[args_idx] = read_input(&input_pos, 0);
    }

    // for (__uint64_t i = 0; i < typ_len; ++i)
    // {
    //     APP_TRACE_INFO((" Executor: i = %lld, val = %lld, type = %lld\n", i, current_call.args_val[i], current_call.args_typ[i]));
    // }

    // return return type
    args_list[kMaxArgs] = read_input(&input_pos, 0);
    APP_TRACE_INFO((" Executor: return type = %lld\n", args_list[kMaxArgs]));

    // read buffer
    __uint64_t data_len = read_input(&input_pos, 0);
    for (__uint64_t args_idx = 0; args_idx < data_len; ++args_idx)
    {
        data_buffer[args_idx] = read_input(&input_pos, 0);
        // APP_TRACE_INFO((" Executor: data index = %lld, data val = %lld\n", args_idx, data_buffer[args_idx]));
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
                APP_TRACE_INFO((" Executor: idx = %lld was a res type; branch 2\n", i));}
            else 
                args_list[i] = current_call.args_val[i];
            break;
        case RES_TYPE_OUT: // 2
            // args_list[i] = current_call.args_val[i];
            off_set = current_call.args_val[i];
            if(ret_buffer[off_set] != 0xffffffff){
                args_list[i] = ret_buffer[current_call.args_val[i]];
                APP_TRACE_INFO((" Executor: idx = %lld was a res type; branch 1\n", i));}
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
            args_list[i] = struct_set[str_id / MAX_ARRAY_LEN] + (str_id % MAX_ARRAY_LEN) * struct_size[str_id / MAX_ARRAY_LEN];
            clr_buffer[clr_idx++] = str_id;
            ret_buffer[str_id % MAX_ARRAY_LEN] = args_list[i];
            break;
        case (GROUP_TYPE ^ PTR_TYPE): // 11
            off_set = current_call.args_val[i];
            args_list[i] = &data_buffer[off_set];
            APP_TRACE_INFO((" Executor: in group: i = %lld, val = %lld\n", i, data_buffer[off_set]));
            break;
        default:
            APP_TRACE_INFO((" Executor: Not Validate Flags"));
            break;
        }
    }

    for (__uint64_t i = 0; i < kMaxArgs; ++i)
    {
        APP_TRACE_INFO((" Executor: arg[%lld] = %lld\n", i, args_list[i]));
    }

    // const
    call_t *call = &syscalls[current_call.syscall_id];

    execute_syscall(call, args_list);
    __uint64_t call_end = read_input(&input_pos, 0);
    kernel_print();
}

__uint64_t read_result(__uint64_t **input_posp)
{
    __uint64_t idx = read_input(input_posp, 0);
    __uint64_t op_div = read_input(input_posp, 0);
    __uint64_t op_add = read_input(input_posp, 0);
    __uint64_t arg = read_input(input_posp, 0);
    if (idx >= kMaxCommands)
        APP_TRACE_INFO((" Executor: command refers to bad result", "result=%lld\n", idx));
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
        APP_TRACE_INFO((" Executor: input command overflows input", "pos=%p: [%p:%p)\n", input_pos,
                        corpus_buffer, corpus_buffer + MAX_PROG_SIZE));
    if (!peek)
        *input_posp = input_pos + 1;
    return *input_pos;
}

void _putchar(char character) { BSP_Ser_WrByte(character); }

void exec_print()
{
    APP_TRACE_INFO((" Executor: ******************** begin exec ************************\n"));
}

void clear_struct()
{
    for (__uint8_t i = 0, idx = 0; i < clr_idx; i++)
    {
        idx = clr_buffer[i];
        Mem_Clr(struct_set[idx / MAX_ARRAY_LEN] + (idx % MAX_ARRAY_LEN) * 
                struct_size[idx / MAX_ARRAY_LEN], struct_size[idx / MAX_ARRAY_LEN]);
    }
    Mem_Clr(clr_buffer, sizeof(__uint8_t) * (clr_idx + 1));
    clr_idx = 0;

    Mem_Set(ret_buffer, 0xffffffff, sizeof(__uint32_t)*20);
}

// extern __uint32_t __start___sancov_guards;
// extern __uint32_t __stop___sancov_guards;
// static void(*sancov_ctor_func)() = 0x80000818; // 0x80000818; 
int main(void)
{
    // sancov_ctor_func();
    // __uint32_t *start = &__start___sancov_guards;
    // __uint32_t *stop = &__stop___sancov_guards;
    // static __uint64_t N; // Counter for the guards.
    // // if (start == stop || *start) return;  // Initialize only once.
    // printf("INIT: %p %p\n", start, stop);
    // for (__uint32_t *x = start; x < stop; x++)
    //     *x = ++N;

    // kernel_print();
    OS_ERR os_err;

    BSP_SysInit();
    BSP_IntInit();
    BSP_OS_TickInit(OSCfg_TickRate_Hz);

    Mem_Init();
    CPU_IntDis();
    CPU_Init();
    Mem_Init();
    DHCPc_Init();
    Math_Init();

    OSInit(&os_err);
    if (os_err != OS_ERR_NONE)
    {
        while (1)
            ;
    }

    App_OS_SetAllHooks();
    OSTaskCreate(&StartupTaskTCB,
                 "Startup Task", StartupTask, 0, APP_CFG_STARTUP_TASK_PRIO,
                 &StartupTaskStk[0u], APP_CFG_TASK_STARTUP_STK_SIZE / 10u,
                 APP_CFG_TASK_STARTUP_STK_SIZE, 0u, 0u, 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), &os_err);
    if (os_err != OS_ERR_NONE)
    {
        while (1)
            ;
    }

    OSStart(&os_err);

    while (DEF_ON)
    {
        ;
    }
}

static void StartupTask(void *p_arg)
{
    APP_TRACE_INFO((" Executor: ******************** TASK start ********************\n"));
    OS_ERR os_err;
#if (APP_CFG_TCPIP_EN > 0u)
    CPU_BOOLEAN status;
#endif
    (void)p_arg;

    OS_TRACE_INIT();

    BSP_OS_TickEnable(); /* Enable the tick timer and interrupt */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(
        &os_err); /* Compute CPU capacity with no task running            */
#endif

    while (DEF_TRUE)
    { 
        if (executor_status[0] == 'r' && executor_status[1] == 'e')
        {

            exec_print();
            serialize_call();
            executor_status[0] = 'w', executor_status[1] = 'a';
            APP_TRACE_INFO((" Executor: current status = %c%c\n\n", executor_status[0], executor_status[1]));
        }
        else if (executor_status[0] == 'c' && executor_status[1] == 'l')
        {
            // reset kernel status
            // exec_print();
            serialize_call();
            APP_TRACE_INFO((" Executor: in the reset logic...\n\n"));
            clear_struct();
            executor_status[0] = 'w', executor_status[1] = 'a';
            APP_TRACE_INFO((" Executor: reset logic has done..."));
        }
        // OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}
