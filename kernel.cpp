#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"

// Since we dont have any OS, it cant perform dynamic linking to the standard library and thus, we cant use functions like printf.
// So, to print we need to put out content on a specific memory location in the RAM 0xb8000. The graphics card automatically prints the contents on screen. We can also set the color information.


void printf(char* str)
{
    // The video memory is partitioned into 16-bit (2-bytes) integers [ fg,bg ][ char ] | [ fg,bg ][ char ] | [ fg,bg ][ char ]
    // First byte is for spefying colors and second byte is for the character to be printed
    // The first 4-bits of the first byte specify the foreground color and the last 4 bits specify the background color
    static uint16_t* VideoMemory = (uint16_t*)0xb8000; // This is declared to take 2 bytes of space because high byte is for color and low byte for aur character. We only need to change the low byte

    static uint8_t x = 0, y = 0;    //Cursor Location
    //Screen is 80 wide x 25 high (characters)
    for(int i = 0; str[i] != '\0'; ++i)     //Increment through each char as long as its not the end symbol
    {
        switch (str[i]) {

            case '\n':      //If newline
                y++;        //New Line
                x = 0;      //Reset Width pos
                break;
            default:        //(This also stops the \n from being printed)
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }
        
        if(x >= 80){    //If at edge of screen
            y++;        //New Line
            x = 0;      //Reset Width pos
        }

        if(y >= 25){  //If at bottom of screen then clear and restart
            for (y = 0; y < 25; ++y) {
                for (x = 0; x < 80; ++x) {
                    //Set everything to a space char
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

// extern "C" tells g++ not to change name of the function when writing in the .o file. 
// Run 'nm file_name.o' to see the symbols to which the compiler has changed name of the functions
// Since we are using extern "C", we can directly write '.extern kernelMain' in our loader.s file and not something like '.extern _Zk13kernelMainEfgh'
extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*magicnumber*/){

    printf("setting up GDT\n");
    GlobalDescriptorTable gdt; // GDT setup
    printf("GDT setup\n");

    printf("setting up interrupts\n");
    InterruptManager interrupts(0x20, &gdt); // IST and PIC setup. (0x20 is the hardware interrupt offset)
    printf("Interrupts setup\n");
    // setup hardware
    KeyboardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);
    interrupts.Activate();

    while(1); // There's no meaning of returning from this function because there's no meaning of kernel finish executing
}