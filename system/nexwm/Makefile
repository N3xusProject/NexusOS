CFILES = $(wildcard src/*.c)

CFLAGS = -std=gnu11           \
	    -ffreestanding       \
	    -fno-stack-protector \
	    -fno-pic             \
	    -mabi=sysv           \
        -Werror-implicit-function-declaration \
        -I../../lib/include/

CC = gcc

.PHONY: all
all: $(CFILES)
	@ mkdir -p obj
	$(CC) -c $(CFLAGS) -Iinclude/libgui/ $^
	mv *.o obj/
