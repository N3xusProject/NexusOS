CFILES = $(wildcard src/*.c)

CFLAGS = -std=gnu11           \
	    -ffreestanding       \
	    -fno-stack-protector \
	    -fno-pic             \
	    -mabi=sysv           \
        -Werror-implicit-function-declaration \
        -I../../lib/include/


LDFLAGS = -nostdlib              \
		-zmax-page-size=0x1000 \
		-static                \
    	-o nexd.sys            \
    	-Tlinker.ld            

CC = gcc

.PHONY: all
all: obj elf

.PHONY: obj
obj: $(CFILES)
	@ mkdir -p obj
	$(CC) -c $(CFLAGS) -Iinclude/ $^
	@ mv *.o obj/

.PHONY: elf
elf:
	ld $(LDFLAGS) obj/*.o ../../lib/obj/* -o nexwm.sys
	@ rm -rf obj
