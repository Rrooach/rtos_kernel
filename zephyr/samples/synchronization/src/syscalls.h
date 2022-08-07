
#define MAX_ARRAY_LEN 8
#define STRUCT_TYPE_NUM 11
struct k_pipe pipe[MAX_ARRAY_LEN];
struct k_heap heap[MAX_ARRAY_LEN];
struct k_stack stack[MAX_ARRAY_LEN];
k_timeout_t tout[MAX_ARRAY_LEN];

const __UINTPTR_TYPE__ struct_set[STRUCT_TYPE_NUM] = {&pipe, &heap, &stack, &tout};

const uint32_t struct_size[STRUCT_TYPE_NUM] = { sizeof(pipe), sizeof(heap), sizeof(stack), sizeof(k_timeout_t)};
 
const call_t syscalls[] = {
  {"crc16", 0, {}, (syscall_t)crc16},
    {"crc16_ansi", 0, {}, (syscall_t)crc16_ansi},
    {"crc16_ccitt", 0, {}, (syscall_t)crc16_ccitt},
    {"crc16_itu_t", 0, {}, (syscall_t)crc16_itu_t},
    {"crc16_reflect", 0, {}, (syscall_t)crc16_reflect},
    {"crc32_c", 0, {}, (syscall_t)crc32_c},
    {"crc32_ieee", 0, {}, (syscall_t)crc32_ieee},
    {"crc32_ieee_update", 0, {}, (syscall_t)crc32_ieee_update},
    {"crc7_be", 0, {}, (syscall_t)crc7_be},
    {"crc8", 0, {}, (syscall_t)crc8},
    {"crc8_ccitt", 0, {}, (syscall_t)crc8_ccitt},
    {"k_aligned_alloc", 0, {}, (syscall_t)k_aligned_alloc},
    {"k_calloc", 0, {}, (syscall_t)k_calloc},
    {"k_free", 0, {}, (syscall_t)k_free},
    {"k_heap_aligned_alloc", 0, {}, (syscall_t)k_heap_aligned_alloc},
    {"k_heap_alloc", 0, {}, (syscall_t)k_heap_alloc},
    {"k_heap_free", 0, {}, (syscall_t)k_heap_free},
    {"k_heap_init", 0, {}, (syscall_t)k_heap_init},
    {"k_malloc", 0, {}, (syscall_t)k_malloc},
    {"k_pipe_alloc_init", 0, {}, (syscall_t)k_pipe_alloc_init},
    {"k_pipe_buffer_flush", 0, {}, (syscall_t)k_pipe_buffer_flush},
    {"k_pipe_cleanup", 0, {}, (syscall_t)k_pipe_cleanup},
    {"k_pipe_flush", 0, {}, (syscall_t)k_pipe_flush},
    {"k_pipe_get", 0, {}, (syscall_t)k_pipe_get},
    {"k_pipe_init", 0, {}, (syscall_t)k_pipe_init},
    {"k_pipe_put", 0, {}, (syscall_t)k_pipe_put},
    {"k_pipe_read_avail", 0, {}, (syscall_t)k_pipe_read_avail},
    {"k_pipe_write_avail", 0, {}, (syscall_t)k_pipe_write_avail},
    {"k_stack_alloc_init", 0, {}, (syscall_t)k_stack_alloc_init},
    {"k_stack_cleanup", 0, {}, (syscall_t)k_stack_cleanup},
    {"k_stack_init", 0, {}, (syscall_t)k_stack_init},
    {"k_stack_pop", 0, {}, (syscall_t)k_stack_pop},
    {"k_stack_push", 0, {}, (syscall_t)k_stack_push},
};
