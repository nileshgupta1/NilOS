
#include <drivers/keyboard.h>

using namespace nilos::common;
using namespace nilos::drivers;
using namespace nilos::hardwarecommunication;



KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::OnKeyDown(char)
{
}

void KeyboardEventHandler::OnKeyUp(char)
{
}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler)
: InterruptHandler(0x21, manager),
dataport(0x60),
commandport(0x64)
// Constructor of KeyboardDriver constructing parent class InterruptHandler with arguments and constructing dataport and commandport objects of the Port8bit class
// Note that 0x60 is the port through which keyboard can send scan codes (data of which key is pressed) to the processor
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);
void printfHex(uint8_t);

void KeyboardDriver::Activate()
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

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    // Reading the scan code from the data port 0x60
    uint8_t key = dataport.Read();
    if(handler == 0)
        return esp;

    // Suppose we press 'a' key, we get two scan codes, 0x1E for key press and 0x9E for key release. We are only recording key press here and therefore we ignore scan codes > 0x80
    // Also note that if we press and hold the 'a' key, we get scan code 0x1E multiple times and at last we get 0x9E only when key is released
    if(key < 0x80)
    {
        switch(key)
        {
            case 0x02: handler->OnKeyDown('1'); break;
            case 0x03: handler->OnKeyDown('2'); break;
            case 0x04: handler->OnKeyDown('3'); break;
            case 0x05: handler->OnKeyDown('4'); break;
            case 0x06: handler->OnKeyDown('5'); break;
            case 0x07: handler->OnKeyDown('6'); break;
            case 0x08: handler->OnKeyDown('7'); break;
            case 0x09: handler->OnKeyDown('8'); break;
            case 0x0A: handler->OnKeyDown('9'); break;
            case 0x0B: handler->OnKeyDown('0'); break;

            case 0x10: handler->OnKeyDown('q'); break;
            case 0x11: handler->OnKeyDown('w'); break;
            case 0x12: handler->OnKeyDown('e'); break;
            case 0x13: handler->OnKeyDown('r'); break;
            case 0x14: handler->OnKeyDown('t'); break;
            case 0x15: handler->OnKeyDown('y'); break; // Note that some keyboards send 0x15 scan code for 'z' and 0x2C for 'y'
            case 0x16: handler->OnKeyDown('u'); break;
            case 0x17: handler->OnKeyDown('i'); break;
            case 0x18: handler->OnKeyDown('o'); break;
            case 0x19: handler->OnKeyDown('p'); break;

            case 0x1E: handler->OnKeyDown('a'); break;
            case 0x1F: handler->OnKeyDown('s'); break;
            case 0x20: handler->OnKeyDown('d'); break;
            case 0x21: handler->OnKeyDown('f'); break;
            case 0x22: handler->OnKeyDown('g'); break;
            case 0x23: handler->OnKeyDown('h'); break;
            case 0x24: handler->OnKeyDown('j'); break;
            case 0x25: handler->OnKeyDown('k'); break;
            case 0x26: handler->OnKeyDown('l'); break;

            case 0x2C: handler->OnKeyDown('z'); break;
            case 0x2D: handler->OnKeyDown('x'); break;
            case 0x2E: handler->OnKeyDown('c'); break;
            case 0x2F: handler->OnKeyDown('v'); break;
            case 0x30: handler->OnKeyDown('b'); break;
            case 0x31: handler->OnKeyDown('n'); break;
            case 0x32: handler->OnKeyDown('m'); break;
            case 0x33: handler->OnKeyDown(','); break;
            case 0x34: handler->OnKeyDown('.'); break;
            case 0x35: handler->OnKeyDown('/'); break;

            case 0x1C: handler->OnKeyDown('\n'); break;
            case 0x39: handler->OnKeyDown(' '); break;

            default:
            {
                printf("KEYBOARD 0x");
                printfHex(key);
                break;
            }
        }
    }
    return esp;
}