BSP_ROOT ?= /root/fuzz/kernel/rt-thread/bsp/qemu-vexpress-a9
RTT_ROOT ?= /root/fuzz/kernel/rt-thread

CROSS_COMPILE ?=/usr/bin/arm-none-eabi-

CFLAGS := -mcpu=cortex-a9 -marm -msoft-float -Wall -g -gdwarf-2 -O0 \
		--target=armv7m-unknown-none-eabi \
		--sysroot=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi \
		-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/include \
		-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/include/lib 

AFLAGS := -c -march=armv7-a -marm -msoft-float -x assembler-with-cpp -D__ASSEMBLY__ -I. -gdwarf-2
LFLAGS := -march=armv7-a -marm -msoft-float -nostartfiles -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,system_vectors -T link.lds
CXXFLAGS := -mcpu=cortex-a9 -marm -msoft-float -Wall -g -gdwarf-2 -O0 \
		--target=armv7m-unknown-none-eabi  \
		--sysroot=/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi \
		-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/include \
		-I/opt/gcc-arm-10.3-2021.07-x86_64-arm-none-eabi/arm-none-eabi/include/lib \
		-Woverloaded-virtual -fno-exceptions -fno-rtti
EXTERN_LIB := -lm -lc 

CPPPATHS :=-I$(BSP_ROOT) \
	-I$(BSP_ROOT)/applications \
	-I$(BSP_ROOT)/drivers \
	-I$(RTT_ROOT)/components/dfs/filesystems/devfs \
	-I$(RTT_ROOT)/components/dfs/filesystems/elmfat \
	-I$(RTT_ROOT)/components/dfs/filesystems/ramfs \
	-I$(RTT_ROOT)/components/dfs/filesystems/romfs \
	-I$(RTT_ROOT)/components/dfs/include \
	-I$(RTT_ROOT)/components/drivers/include \
	-I$(RTT_ROOT)/components/drivers/spi \
	-I$(RTT_ROOT)/components/drivers/spi/sfud/inc \
	-I$(RTT_ROOT)/components/finsh \
	-I$(RTT_ROOT)/components/libc/compilers/common \
	-I$(RTT_ROOT)/components/libc/compilers/newlib \
	-I$(RTT_ROOT)/components/libc/cplusplus \
	-I$(RTT_ROOT)/components/libc/posix/delay \
	-I$(RTT_ROOT)/components/libc/posix/io/aio \
	-I$(RTT_ROOT)/components/libc/posix/io/poll \
	-I$(RTT_ROOT)/components/libc/posix/io/stdio \
	-I$(RTT_ROOT)/components/libc/posix/io/termios \
	-I$(RTT_ROOT)/components/libc/posix/ipc \
	-I$(RTT_ROOT)/components/libc/posix/pthreads \
	-I$(RTT_ROOT)/components/libc/posix/signal \
	-I$(RTT_ROOT)/components/lwp \
	-I$(RTT_ROOT)/components/net/lwip/lwip-2.0.3/src/include \
	-I$(RTT_ROOT)/components/net/lwip/lwip-2.0.3/src/include/ipv4 \
	-I$(RTT_ROOT)/components/net/lwip/lwip-2.0.3/src/include/netif \
	-I$(RTT_ROOT)/components/net/lwip/port \
	-I$(RTT_ROOT)/components/net/netdev/include \
	-I$(RTT_ROOT)/components/net/sal/impl \
	-I$(RTT_ROOT)/components/net/sal/include \
	-I$(RTT_ROOT)/components/net/sal/include/dfs_net \
	-I$(RTT_ROOT)/components/net/sal/include/socket \
	-I$(RTT_ROOT)/components/net/sal/include/socket/sys_socket \
	-I$(RTT_ROOT)/include \
	-I$(RTT_ROOT)/libcpu/arm/common \
	-I$(RTT_ROOT)/libcpu/arm/cortex-a 

DEFINES := -DHAVE_CCONFIG_H -DRT_USING_NEWLIB -D_POSIX_C_SOURCE=1 -D__RTTHREAD__
