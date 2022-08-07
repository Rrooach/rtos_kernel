#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <stdio.h>

#include "riscv-virt.h"
#include "ns16550.h" 

#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 ) 
#define mainQUEUE_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 1000 ) 
#define mainQUEUE_LENGTH					( 1 )

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/*-----------------------------------------------------------*/


#define SYSCALLAPI
#define ALIGNED(N) __attribute__((aligned(N)))
#define INPUT_DATA_ALIGNMENT 64 << 10
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define kMaxArgs 13
#define kCoverSize 256 << 10
#define kMaxCommands 1000
#define instr_eof -1
#define MAX_PROG_SIZE (2 << 15)
#define MAX_DATA_SIZE (2 << 15)

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

// static res_t results[kMaxCommands];

#include "executor_freeRTOS.h"
#include "syscalls.h"

void kernel_print()
{
    printf((" Executor: ******************** OS end ************************\n"));
}

__uint64_t read_input(__uint64_t **input_posp, __uint32_t peek)
{
    __uint64_t *input_pos = *input_posp;
    if ((__uint8_t *)input_pos >= corpus_buffer + MAX_PROG_SIZE)
        printf("input command overflows input", "pos=%p: [%p:%p)\n", input_pos,
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
    // // Mem_Set(ret_buffer, 0xffffffff, sizeof(__uint32_t)*20);
    

    // current_call.syscall_id = read_input(&input_pos, 0);
    if (current_call.syscall_id == instr_eof)
    {
        printf("exit because of eof");
        // APP_TRACE_INFO((", the syascall id is %lld\n", current_call.syscall_id));
        return;
    }

    // printf("name = %s\n", syscalls[current_call.syscall_id].name); 

    // // Abnormal syscall.
    if (current_call.syscall_id >= ARRAY_SIZE(syscalls))
        printf("call_num=%llu is invalid\n", current_call.syscall_id);

    // // read val array
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

    // for (__uint64_t i = 0; i < typ_len; ++i)
    // {
    //     APP_TRACE_INFO(("i = %lld, val = %lld, type = %lld\n", i, current_call.args_val[i], current_call.args_typ[i]));
    // }

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
    
    read_input(&input_pos, 0);

    kernel_print();
}

/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{ 
	unsigned long ulReceivedValue;
	const unsigned long ulExpectedValue = 100UL;
	const char * const pcMessage1 = "begin execute";
	const char * const pcMessage2 = "entering execute";
	const char * const pcFailMessage = "Unexpected value received\r\n";
	int f = 1;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

    vSendString( executor_status ); 
	for( ;; )
	{ 
        misc[1] = '0';
		vSendString( executor_status );
		 if (executor_status[0] == 'r' && executor_status[1] == 'e')
        {
            vSendString( pcMessage1 );
            serialize_call();
            executor_status[0] = 'w', executor_status[1] = 'a';
            printf(" Executor: current status = %c%c\n\n", executor_status[0], executor_status[1]);
        }
		else if (executor_status[0] == 'c' && executor_status[1] == 'l')
        {
            // reset kernel status
            vSendString( pcMessage1 );
            serialize_call();
            printf(" Executor: in the reset logic...\n\n");
            // clear_struct();
            executor_status[0] = 'w', executor_status[1] = 'a';
            printf(" Executor: reset logic has done...");
        }
 
	}
}

int main_blinky( void )
{
     /* Initialise the hardware ready for the demo. */     
	vSendString( "Hello FreeRTOS!" );
	  
	xTaskCreate( prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE * 2U, NULL,
				mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
	vTaskStartScheduler();

	return 0;
} 