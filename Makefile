

GCCPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings # tell the compiler that there is no OS or the glibc standard library to which it can link or use extra functionalities, exceptions, etc
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o gdt.o driver.o port.o interruptstubs.o interrupts.o keyboard.o mouse.o kernel.o

# Syntax of a Makefile
# target: prerequisites
# <TAB>command
# The first target in a Makefile is the default target executed when we run 'make'. For example build, test and clean targets get run:
# all: build test clean
# build:
# 	gcc -o main main.c

# to create a .o file from a .cpp file (we need the cpp files) and will use: g++ (param for 32-bit mode) output will be (target file) and compile the (input file)



%.o: %.cpp
	gcc $(GCCPARAMS) -c -o $@ $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

nilos.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

nilos.iso: nilos.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/nilos.bin'    >> iso/boot/grub/grub.cfg
	echo '	boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

run: nilos.iso
	qemu-system-i386 -cdrom $<

.PHONY: clean
clean:
	rm -f $(objects) nilos.bin nilos.iso