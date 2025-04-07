#include "types.h"

// Since we dont have any OS, it cant perform dynamic linking to the standard library and thus, we cant use functions like printf.
// So, to print we need to put out content on a specific memory location in the RAM 0xb8000. The graphics card automatically prints the contents on screen. We can also set the color information.


void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000; // This is declared as unsigned short so that each element takes 2 bytes of space because high byte is for color and low byte for aur character. We only need to change the low byte

    for(int i = 0; str[i] != '\0'; ++i)
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i]; // Only changes the low byte. VideoMemory[i] changes from [(high byte)(low byte)] ---> [(high byte), (str[i])] (2bytes).
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernelMain(void* multiboot_structure, unsigned int magicnumber){  // extern "C" tells g++ not to change name of the function when writing in the .o file
    printf("NilOS kernel");
    while(1); // There's no meaning of returning from this function because there's no meaning of kernel finish executing
}