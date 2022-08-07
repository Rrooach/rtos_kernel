// Copyright 2017 syzkaller project authors. All rights reserved.
// Use of this source code is governed by Apache 2 LICENSE that can be found in the LICENSE file.
 

static void os_init(int argc, char** argv, void* data, size_t data_size)
{
	// zx_status_t status = syz_mmap((size_t)data, data_size);
	// if (status != ZX_OK)
	// 	failmsg("mmap of data segment failed", "status=%s (%d)", zx_status_get_string(status), status);
}

static void execute_syscall( call_t* c, __uint64_t a[kMaxArgs+1])
{
	intptr_t* res = c->call(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12]);
	
	// We cast libc functions to signature returning intptr_t,
	// as the result int -1 is returned as 0x00000000ffffffff rather than full -1.
  if(a[kMaxArgs] != 0xffffffff)
    ret_buffer[a[kMaxArgs]] = res;
}


#define MAX_SIZE (1<<16) 
#define ROL32(_x, _r)  ((((__UINT32_TYPE__)(_x)) << (_r)) | (((__UINT32_TYPE__)(_x)) >> (32 - (_r))))

 __UINT8_TYPE__ BB_COVER[MAX_SIZE]; //= {0};
 __UINT8_TYPE__ EG_COVER[MAX_SIZE]; //= {0};
static __UINT32_TYPE__ bb_number = 0;
static __UINT32_TYPE__ pre_bb = 0, start_brach = 0;

static inline __UINT32_TYPE__ hash32(const void* key, __UINT32_TYPE__ len, __UINT32_TYPE__ seed) {

  const __UINT32_TYPE__* data  = (__UINT32_TYPE__*)key;
  __UINT32_TYPE__ h1 = seed ^ len;

  len >>= 2;

  while (len--) {

    __UINT32_TYPE__ k1 = *data++;

    k1 *= 0xcc9e2d51;
    k1  = ROL32(k1, 15);
    k1 *= 0x1b873593;

    h1 ^= k1;
    h1  = ROL32(h1, 13);
    h1  = h1 * 5 + 0xe6546b64;

  }

  h1 ^= h1 >> 16;
  h1 *= 0x85ebca6b;
  h1 ^= h1 >> 13;
  h1 *= 0xc2b2ae35;
  h1 ^= h1 >> 16;

  return h1;

}

// extern void kernel_print();

extern void __sanitizer_cov_trace_pc_guard(__UINT32_TYPE__* guard) {
  if (!*guard) return;
  // if (BB_COVER[*guard] == 0) {
  //   bb_number++;
  // }

  // BB_COVER[*guard]++;
  EG_COVER[*guard]++; 

}


extern void __sanitizer_cov_trace_pc_guard_init(__UINT32_TYPE__ *start,
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
