.PHONY: all
all:
	gcc -c -std=gnu11           \
	    -ffreestanding       \
	    -fno-stack-protector \
	    -fno-pic             \
	    -mabi=sysv           \
        -Werror-implicit-function-declaration \
	    -mno-80387           \
	    -mno-mmx             \
	    -mno-3dnow           \
	    -mno-sse             \
	    -mno-sse2            \
	    -mno-red-zone        \
	    -mcmodel=kernel      \
	    -MMD                 \
	    -Tlinker.ld			 \
	    src/main.c -o main.o
	objcopy -O binary -j .text main.o nexd.bin
	# rm *.o *.d
