#include "/root/fuzz/kernel/freeRTOS/FreeRTOS/FreeRTOS/Source/include/task.h"
#include <timers.h>
#include <task.h>
#define MAX_ARRAY_LEN 8
#define STRUCT_TYPE_NUM 11
 


const __UINTPTR_TYPE__ struct_set[STRUCT_TYPE_NUM]; // = { &mem_dyn_pool, &os_tmr, &os_mem, 
                                    // &mem_pool, &os_tcb, &os_flag_grp, 
                                    // &os_mutex, &os_sem, &os_q, &mem_seg, 
                                    // &net_if};

// const CPU_SIZE_T struct_size[STRUCT_TYPE_NUM] = { sizeof(mem_dyn_pool), sizeof(os_tmr), sizeof(os_mem), 
//                                     sizeof(mem_pool), sizeof(os_tcb), sizeof(os_flag_grp), 
//                                     sizeof(os_mutex), sizeof(os_sem), sizeof(os_q), sizeof(mem_seg), 
//                                     sizeof(net_if)};

const __UINT32_TYPE__ struct_size[STRUCT_TYPE_NUM]; // = { sizeof(mem_dyn_pool[0]), sizeof(os_tmr[0]), sizeof(os_mem[0]), sizeof(mem_pool[0]), sizeof(os_tcb[0]), sizeof(os_flag_grp[0]), sizeof(os_mutex[0]), sizeof(os_sem[0]), sizeof(os_q[0]), sizeof(mem_seg[0]), sizeof(net_if[0])};

const call_t syscalls[] = {
    {"pcTimerGetName", 0, {}, (syscall_t)pcTimerGetName},
    {"pvTimerGetTimerID", 0, {}, (syscall_t)pvTimerGetTimerID},  
    {"uxTaskGetNumberOfTasks", 0, {}, (syscall_t)uxTaskGetNumberOfTasks},
    {"uxTimerGetReloadMode", 0, {}, (syscall_t)uxTimerGetReloadMode},
    // {"vTaskGetRunTimeStats", 0, {}, (syscall_t)vTaskGetRunTimeStats},
    // {"vTaskList", 0, {}, (syscall_t)vTaskList},
    {"vTaskSetTimeOutState", 0, {}, (syscall_t)vTaskSetTimeOutState},
    {"vTimerSetReloadMode", 0, {}, (syscall_t)vTimerSetReloadMode},
    {"vTimerSetTimerID", 0, {}, (syscall_t)vTimerSetTimerID},
    {"xTaskCheckForTimeOut", 0, {}, (syscall_t)xTaskCheckForTimeOut},
    {"xTaskGetCurrentTaskHandle", 0, {}, (syscall_t)xTaskGetCurrentTaskHandle},
    {"xTaskGetHandle", 0, {}, (syscall_t)xTaskGetHandle},
    // {"xTaskGetIdleRunTimeCounter", 0, {}, (syscall_t)xTaskGetIdleRunTimeCounter},
    {"xTaskGetSchedulerState", 0, {}, (syscall_t)xTaskGetSchedulerState},
    {"xTaskGetTickCount", 0, {}, (syscall_t)xTaskGetTickCount},
    {"xTaskGetTickCountFromISR", 0, {}, (syscall_t)xTaskGetTickCountFromISR}, 
    {"xTimerCreate", 0, {}, (syscall_t)xTimerCreate},
    {"xTimerGetExpiryTime", 0, {}, (syscall_t)xTimerGetExpiryTime},
    {"xTimerGetPeriod", 0, {}, (syscall_t)xTimerGetPeriod},
    {"xTimerGetTimerDaemonTaskHandle", 0, {}, (syscall_t)xTimerGetTimerDaemonTaskHandle},
    {"xTimerIsTimerActive", 0, {}, (syscall_t)xTimerIsTimerActive},
    {"xTimerPendFunctionCall", 0, {}, (syscall_t)xTimerPendFunctionCall}, 
};
