
static void execute_syscall( call_t* c, __uint64_t a[kMaxArgs+1])
{
    intptr_t* res = (intptr_t*)c->call(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12]);

    if(a[kMaxArgs] != 0xffffffff)
    ret_buffer[a[kMaxArgs]] = (intptr_t)res;
}

#define MAX_SIZE (1<<15)  
 __UINT8_TYPE__ BB_COVER[MAX_SIZE]; //= {0};
 __UINT8_TYPE__ EG_COVER[MAX_SIZE]; //= {0};
static __UINT32_TYPE__ bb_number = 0; 

void __sanitizer_cov_trace_pc_guard(__UINT32_TYPE__* guard) {
  if (!*guard) return;
  // if (BB_COVER[*guard] == 0) {
  //   bb_number++;
  // }

  // BB_COVER[*guard]++;
  EG_COVER[*guard]++; 

}

void __sanitizer_cov_trace_pc_guard_init(__UINT32_TYPE__ *start,
                                         __UINT32_TYPE__ *stop) {
    for (int i = 0; i < MAX_SIZE; ++i) 
      BB_COVER[i] = 255, EG_COVER[i] = 0;
                  
    static __UINT64_TYPE__ N = 0;  // Counter for the guards.
    if (start == stop /*|| *start*/) return;  // Initialize only once. 
// if (start == stop || *start) return;  // Initialize only once. 
    for (__UINT32_TYPE__ *x = start; x < stop; x++) {
      *x = ++N; 
    }
     
}
