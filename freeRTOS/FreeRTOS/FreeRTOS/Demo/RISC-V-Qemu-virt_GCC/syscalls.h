#include <timers.h>
#include <task.h>
#define MAX_ARRAY_LEN 8
#define STRUCT_TYPE_NUM 11

typedef struct tmrTimerControl                  
{
    const char * pcTimerName;                   
    ListItem_t xTimerListItem;                  
    TickType_t xTimerPeriodInTicks;             
    void * pvTimerID;                           
    TimerCallbackFunction_t pxCallbackFunction; 
    #if ( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxTimerNumber;              
    #endif
    uint8_t ucStatus;                           
} xTIMER;
 
typedef struct tskTaskControlBlock       
{
    volatile StackType_t * pxTopOfStack; 
    #if ( portUSING_MPU_WRAPPERS == 1 )
        xMPU_SETTINGS xMPUSettings; 
    #endif
    ListItem_t xStateListItem;      
    ListItem_t xEventListItem;      
    UBaseType_t uxPriority;
    StackType_t * pxStack;
    char pcTaskName[ configMAX_TASK_NAME_LEN ]; 
    #if ( ( portSTACK_GROWTH > 0 ) || ( configRECORD_STACK_HIGH_ADDRESS == 1 ) )
        StackType_t * pxEndOfStack; 
    #endif
    #if ( portCRITICAL_NESTING_IN_TCB == 1 )
        UBaseType_t uxCriticalNesting; 
    #endif
    #if ( configUSE_TRACE_FACILITY == 1 )
        UBaseType_t uxTCBNumber;
        UBaseType_t uxTaskNumber;
    #endif
    #if ( configUSE_MUTEXES == 1 )
        UBaseType_t uxBasePriority;
        UBaseType_t uxMutexesHeld;
    #endif
    #if ( configUSE_APPLICATION_TASK_TAG == 1 )
        TaskHookFunction_t pxTaskTag;
    #endif
    #if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
        void * pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
    #endif
    #if ( configGENERATE_RUN_TIME_STATS == 1 )
        uint32_t ulRunTimeCounter; 
    #endif
    #if ( configUSE_NEWLIB_REENTRANT == 1 )
        struct  _reent xNewLib_reent;
    #endif
    #if ( configUSE_TASK_NOTIFICATIONS == 1 )
        volatile uint32_t ulNotifiedValue[ configTASK_NOTIFICATION_ARRAY_ENTRIES ];
        volatile uint8_t ucNotifyState[ configTASK_NOTIFICATION_ARRAY_ENTRIES ];
    #endif
    #if ( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
        uint8_t ucStaticallyAllocated;                   
    #endif
    #if ( INCLUDE_xTaskAbortDelay == 1 )
        uint8_t ucDelayAborted;
    #endif
    #if ( configUSE_POSIX_ERRNO == 1 )
        int iTaskErrno;
    #endif
} tskTCB;

typedef tskTCB TCB_t;
typedef xTIMER Timer_t;
 
TaskStatus_t rtxTASK_STATUS[MAX_ARRAY_LEN];
TimeOut_t rtxTIME_OUT[MAX_ARRAY_LEN];
Timer_t rttmrTimerControl[MAX_ARRAY_LEN];
TCB_t rttskTaskControlBlock[MAX_ARRAY_LEN];

const __UINTPTR_TYPE__ struct_set[STRUCT_TYPE_NUM]  = { &rtxTASK_STATUS, &rtxTIME_OUT, 
                                &rttmrTimerControl, &rttskTaskControlBlock};

const __UINT32_TYPE__ struct_size[STRUCT_TYPE_NUM] = { sizeof(rtxTASK_STATUS), sizeof(rtxTIME_OUT), 
                                sizeof(rttmrTimerControl), sizeof(rttskTaskControlBlock)};
 
const call_t syscalls[] = {
    {"pcTimerGetName", 0, {}, (syscall_t)pcTimerGetName},
    {"pvTimerGetTimerID", 0, {}, (syscall_t)pvTimerGetTimerID},
    {"uxTaskGetNumberOfTasks", 0, {}, (syscall_t)uxTaskGetNumberOfTasks},
    {"uxTimerGetReloadMode", 0, {}, (syscall_t)uxTimerGetReloadMode},
    {"vTaskDelete", 0, {}, (syscall_t)vTaskDelete},
    {"vTaskGetInfo", 0, {}, (syscall_t)vTaskGetInfo},
    {"vTaskSetTimeOutState", 0, {}, (syscall_t)vTaskSetTimeOutState},
    {"vTimerSetReloadMode", 0, {}, (syscall_t)vTimerSetReloadMode},
    {"vTimerSetTimerID", 0, {}, (syscall_t)vTimerSetTimerID},
    {"xTaskCallApplicationTaskHook", 0, {}, (syscall_t)xTaskCallApplicationTaskHook},
    {"xTaskCheckForTimeOut", 0, {}, (syscall_t)xTaskCheckForTimeOut},
    {"xTaskCreate", 0, {}, (syscall_t)xTaskCreate},
    {"xTaskGetApplicationTaskTag", 0, {}, (syscall_t)xTaskGetApplicationTaskTag},
    {"xTaskGetApplicationTaskTagFromISR", 0, {}, (syscall_t)xTaskGetApplicationTaskTagFromISR},
    {"xTaskGetCurrentTaskHandle", 0, {}, (syscall_t)xTaskGetCurrentTaskHandle},
    {"xTaskGetHandle", 0, {}, (syscall_t)xTaskGetHandle},
    {"xTaskGetSchedulerState", 0, {}, (syscall_t)xTaskGetSchedulerState},
    {"xTaskGetTickCount", 0, {}, (syscall_t)xTaskGetTickCount},
    {"xTaskGetTickCountFromISR", 0, {}, (syscall_t)xTaskGetTickCountFromISR},
    {"xTimerCreate", 0, {}, (syscall_t)xTimerCreate},
    {"xTimerGenericCommand$xTimerChangePeriod", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGenericCommand$xTimerChangePeriodFromISR", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGenericCommand$xTimerDelete", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGenericCommand$xTimerReset", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGenericCommand$xTimerResetFromISR", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGenericCommand$xTimerStart", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGenericCommand$xTimerStartFromISR", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGenericCommand$xTimerStop", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGenericCommand$xTimerStopFromISR", 0, {}, (syscall_t)xTimerGenericCommand},
    {"xTimerGetExpiryTime", 0, {}, (syscall_t)xTimerGetExpiryTime},
    {"xTimerGetPeriod", 0, {}, (syscall_t)xTimerGetPeriod},
    {"xTimerGetTimerDaemonTaskHandle", 0, {}, (syscall_t)xTimerGetTimerDaemonTaskHandle},
    {"xTimerIsTimerActive", 0, {}, (syscall_t)xTimerIsTimerActive},
};