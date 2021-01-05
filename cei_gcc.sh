#!bin/sh
clear
gcc -o cei cei.c -s -O3 -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles --entry=cei
test -f cei.exe || exit
wc -c cei.exe
strip --strip-all cei.exe
wc -c cei.exe
