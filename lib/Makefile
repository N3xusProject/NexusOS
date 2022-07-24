LIBGUI_FILES = $(wildcard src/libgui/*.c)

CFLAGS = -std=gnu11           \
	    -ffreestanding       \
	    -fno-stack-protector \
	    -fno-pic             \
	    -mabi=sysv           \
        -Werror-implicit-function-declaration \

CC = gcc

.PHONY: libgui
libgui: $(LIBGUI_FILES)
	@ mkdir -p obj
	$(CC) -c $(CFLAGS) -Iinclude/libgui/ $^
	mv *.o obj/
