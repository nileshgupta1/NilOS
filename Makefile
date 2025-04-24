
# using -Iinclude tells the compiler to look for include files in the include directory and therefore we use #include <common/types.h> instead of #include "path/to/types.h"
GCCPARAMS = -m32  -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings # tell the compiler that there is no OS or the glibc standard library to which it can link or use extra functionalities, exceptions, etc
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
		  obj/gdt.o \
		  obj/drivers/driver.o \
		  obj/hardwarecommunication/port.o \
		  obj/hardwarecommunication/interruptstubs.o \
		  obj/hardwarecommunication/interrupts.o \
		  obj/hardwarecommunication/pci.o \
		  obj/drivers/keyboard.o \
		  obj/drivers/mouse.o \
		  obj/kernel.o

# Syntax of a Makefile
# target: prerequisites
# <TAB>command
# The first target in a Makefile is the default target executed when we run 'make'. For example build, test and clean targets get run:
# all: build test clean
# build:
# 	gcc -o main main.c

# to create a .o file from a .cpp file (we need the cpp files) and will use: g++ (param for 32-bit mode) output will be (target file) and compile the (input file)



obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

nilos.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

nilos.iso: nilos.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot
	echo 'menuentry "NilOS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/nilos.bin'    >> iso/boot/grub/grub.cfg
	echo '	boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

run: nilos.iso
	qemu-system-i386 -cdrom $<

.PHONY: clean
clean:
	rm -rf obj nilos.bin nilos.iso