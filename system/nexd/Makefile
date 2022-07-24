.PHONY: all
all:
	cd ../../lib; make
	cd ../nexwm/; make
	gcc -c -std=gnu11           \
	    -ffreestanding       \
	    -fno-stack-protector \
	    -fno-pic             \
	    -mabi=sysv           \
        -Werror-implicit-function-declaration \
	    -MMD                 \
	    -I../../lib/include/ -I../nexwm/include/ \
	    src/main.c -o main.o 
	ld \
		-nostdlib              \
		-zmax-page-size=0x1000 \
		-static                \
    	-o nexd.sys            \
    	-Tlinker.ld            \
    	main.o ../nexwm/obj/*.o ../../lib/obj/*
	# objcopy -O binary -j .text main.o nexd.bin
	rm *.o *.d


