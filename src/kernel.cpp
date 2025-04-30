#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>


// #define GRAPHICSMODE // Uncomment to activate VGA graphics mode


using namespace nilos;
using namespace nilos::common;
using namespace nilos::drivers;
using namespace nilos::hardwarecommunication;
using namespace nilos::gui;

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


void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}

// Used for printing on the console the key pressed using the VideoMemory method. We may use different printing method in other modes such as GUI
class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        char* foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:
    
    MouseToConsole()
    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);  
    }
    
    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        // Flip the foregroung and background colors of the character so that it looks like our cursor is present at that position
        // Interchanging foreground and background colors by swapping the first 4 bits and second 4 bits, keeping last 8 bits same
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x >= 80) x = 79;
        if(x < 0) x = 0;
        y += yoffset;
        if(y >= 25) y = 24;
        if(y < 0) y = 0;

        // Flip the foregroung and background colors of the character so that it looks like our cursor is present at that position
        // Interchanging foreground and background colors by swapping the first 4 bits and second 4 bits, keeping last 8 bits same
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }
    
};





void taskA()
{
    while(true)
        printf("A");
}
void taskB()
{
    while(true)
        printf("B");
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

    GlobalDescriptorTable gdt; // GDT setup

    // We can get to know about the available memory from BIOS through the multiboot structure provided by the bootloader to the kernel.
    // The multiboot.h from GNU project provides us with the structure of the multiboot structure
    // The multiboot_info struct contains an integer mem_upper (8 bytes from the start of the struct) which tells us the size of the RAM (in KiB).
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024; //  address where heap starts
    // MemoryMAnager constructor takes in parameters the address where the heap starts and the size of the heap
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024); // memupper is in KB so we multiply by 1024. Substract the address where heap starts heap size and also 10KB padding behind the heap

    // printing the heap address and the address of the allocated memory
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8 ) & 0xFF);
    printfHex((heap      ) & 0xFF);
    // prints 0x00A00000 = 1,048,576B = 10*1024*1024, so heap starts at 10 megabytes
    
    void* allocated = memoryManager.malloc(1024);
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8 ) & 0xFF);
    printfHex(((size_t)allocated      ) & 0xFF);
    printf("\n");
    // prints 0x00A00010, so memory allocated at 10*1024*1024 + 16, so 16 bytes after the heap address

    TaskManager taskManager;
    /*
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    */

    InterruptManager interrupts(0x20, &gdt, &taskManager); // IST and PIC setup, (0x20 is the hardware interrupt offset), Scheduler of taskManager called in interrupt is a timer interrupt

    printf("Initializing Hardware, Stage 1\n");

    #ifdef GRAPHICSMODE
        Desktop desktop(320,200, 0x00,0x00,0xA8);
    #endif
    
    DriverManager drvManager;

        #ifdef GRAPHICSMODE
            KeyboardDriver keyboard(&interrupts, &desktop);
        #else
            PrintfKeyboardEventHandler kbhandler;
            KeyboardDriver keyboard(&interrupts, &kbhandler);
        #endif
        drvManager.AddDriver(&keyboard);


        #ifdef GRAPHICSMODE
            MouseDriver mouse(&interrupts, &desktop);
        #else
            MouseToConsole mousehandler;
            MouseDriver mouse(&interrupts, &mousehandler);
        #endif
        drvManager.AddDriver(&mouse);
        
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);

        VideoGraphicsArray vga;

    printf("Initializing Hardware, Stage 2\n");
        drvManager.ActivateAll();

    printf("Initializing Hardware, Stage 3\n");
    
    #ifdef GRAPHICSMODE
        vga.SetMode(320,200,8);
        Window win1(&desktop, 10,10,20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);
        Window win2(&desktop, 40,15,30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif


    interrupts.Activate();

    while(1)
    {
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}