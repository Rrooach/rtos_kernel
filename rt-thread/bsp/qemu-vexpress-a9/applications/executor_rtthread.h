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
	intptr_t* res = (intptr_t *)c->call(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12]);
	
	// We cast libc functions to signature returning intptr_t,
	// as the result int -1 is returned as 0x00000000ffffffff rather than full -1.
  if(a[kMaxArgs] != 0xffffffff)
    ret_buffer[a[kMaxArgs]] = (__UINT32_TYPE__)res;
}
 