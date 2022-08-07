
static void execute_syscall( call_t* c, __uint64_t a[kMaxArgs+1])
{
    intptr_t* res = (intptr_t*)c->call(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12]);

    if(a[kMaxArgs] != 0xffffffff)
    ret_buffer[a[kMaxArgs]] = (intptr_t)res;
}

 