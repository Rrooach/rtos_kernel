set architecture i386:x86-64
set pagination off
set logging file gdb.txt
set logging on
target remote:8999
p &test
set logging off
quit
