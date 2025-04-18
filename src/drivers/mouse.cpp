
#include <drivers/mouse.h>


using namespace nilos::common;
using namespace nilos::drivers;
using namespace nilos::hardwarecommunication;


MouseEventHandler::MouseEventHandler()
{
}

void MouseEventHandler::OnActivate()
{
}

void MouseEventHandler::OnMouseDown(uint8_t button)
{
}

void MouseEventHandler::OnMouseUp(uint8_t button)
{
}

void MouseEventHandler::OnMouseMove(int x, int y)
{
}

MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(0x2C, manager),
dataport(0x60),
commandport(0x64)
{
    this->handler = handler;
}

MouseDriver::~MouseDriver()
{
}

void MouseDriver::Activate()
{
    offset = 0;
    buttons = 0;


    if(handler != 0)
        handler->OnActivate();

    while (commandport.Read() & 0x1) 
        dataport.Read(); 
    commandport.Write(0xA8); // activate Mouse interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 2); // Set second bit to true
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4); // Mouse activated
    dataport.Read();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    // Return if data is not available (data is available if 6th bit is set)
    if(!(status & 0x20)) return esp; 


    buffer[offset] = dataport.Read();

    if(handler == 0) return esp;
    
    if(++offset == 3) offset = 0;

    // buffer[1] is the movement on the  x-axis and buffer[2] is the movement on the y-axis (in opposite directions)
    if(offset == 0)
    {
        if(buffer[1] != 0 || buffer[2] != 0)
        {
            handler->OnMouseMove(buffer[1], -buffer[2]);
        }
        for(uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
            {
                if(buttons & (0x1<<i))
                    handler->OnMouseUp(i+1);
                else
                    handler->OnMouseDown(i+1);
            }
        }
        buttons = buffer[0];
    }

    return esp;
}