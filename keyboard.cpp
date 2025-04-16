
#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
dataport(0x60),
commandport(0x64)
// Constructor of KeyboardDriver constructing parent class InterruptHandler with arguments and constructing dataport and commandport objects of the Port8bit class
// Note that 0x60 is the port through which keyboard can send scan codes (data of which key is pressed) to the processor
{
    // Removes all the keystrikes that might have been there before
    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xAE); // activate keyboard interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10; // Change the controller state and write it back. Set lrightmost bit to 1 (new state) and clear the 5th bit
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    dataport.Write(0xf4); // Keyboard activated
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    // Reading the scan code from the data port 0x60
    uint8_t key = dataport.Read();
    // Suppose we press 'a' key, we get two scan codes, 0x1E for key press and 0x9E for key release. We are only recording key press here and therefore we ignore scan codes > 0x80
    // Also note that if we press and hold the 'a' key, we get scan code 0x1E multiple times and at last we get 0x9E only when key is released
    if(key < 0x80)
    {
        switch(key)
        {
            case 0x02: printf("1"); break;
            case 0x03: printf("2"); break;
            case 0x04: printf("3"); break;
            case 0x05: printf("4"); break;
            case 0x06: printf("5"); break;
            case 0x07: printf("6"); break;
            case 0x08: printf("7"); break;
            case 0x09: printf("8"); break;
            case 0x0A: printf("9"); break;
            case 0x0B: printf("0"); break;

            case 0x10: printf("q"); break;
            case 0x11: printf("w"); break;
            case 0x12: printf("e"); break;
            case 0x13: printf("r"); break;
            case 0x14: printf("t"); break;
            case 0x15: printf("y"); break; // Note that some keyboards send 0x15 scan code for 'z' and 0x2C for 'y'
            case 0x16: printf("u"); break;
            case 0x17: printf("i"); break;
            case 0x18: printf("o"); break;
            case 0x19: printf("p"); break;

            case 0x1E: printf("a"); break;
            case 0x1F: printf("s"); break;
            case 0x20: printf("d"); break;
            case 0x21: printf("f"); break;
            case 0x22: printf("g"); break;
            case 0x23: printf("h"); break;
            case 0x24: printf("j"); break;
            case 0x25: printf("k"); break;
            case 0x26: printf("l"); break;

            case 0x2C: printf("z"); break;
            case 0x2D: printf("x"); break;
            case 0x2E: printf("c"); break;
            case 0x2F: printf("v"); break;
            case 0x30: printf("b"); break;
            case 0x31: printf("n"); break;
            case 0x32: printf("m"); break;
            case 0x33: printf(","); break;
            case 0x34: printf("."); break;
            case 0x35: printf("/"); break;

            case 0x1C: printf("\n"); break;
            case 0x39: printf(" "); break;

            default:
            {
                char* foo = "KEYBOARD 0x00 ";
                char* hex = "0123456789ABCDEF";
                foo[11] = hex[(key >> 4) & 0xF];
                foo[12] = hex[key & 0xF];
                printf(foo);
                break;
            }
        }
    }
    return esp;
}