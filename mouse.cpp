
#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager* manager)
: InterruptHandler(0x2C, manager),
dataport(0x60),
commandport(0x64)
{
    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    offset = 0;
    buttons = 0;
    x = 40;
    y = 12;

    // At the startup flip the foregroung and background colors of the character at the centre of the screen (x = 40, y = 12) so that it looks like our cursor is present at that position
    // Interchanging foreground and background colors by swapping the first 4 bits and second 4 bits, keeping last 8 bits same
    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

    commandport.Write(0xA8); // activate Mouse interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 2); // Set second bit to true
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4); // Mouse activated
    dataport.Read();
}

MouseDriver::~MouseDriver()
{
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    // Return if data is not available (data is available if 6th bit is set)
    if(!(status & 0x20)) return esp; 


    buffer[offset] = dataport.Read();
    if(++offset == 3) offset = 0;

    // buffer[1] is the movement on the  x-axis and buffer[2] is the movement on the y-axis (in opposite directions)
    if(offset == 0)
    {
        if(buffer[1] != 0 || buffer[2] != 0)
        {
            // The video memory is partitioned into 16-bit (2-bytes) integers [ fg,bg ][ char ] | [ fg,bg ][ char ] | [ fg,bg ][ char ]
            // First byte is for spefying colors and second byte is for the character to be printed
            // The first 4-bits of the first byte specify the foreground color and the last 4 bits specify the background color
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;

            // Flip the foregroung and background colors of the character so that it looks like our cursor is present at that position
            // Interchanging foreground and background colors by swapping the first 4 bits and second 4 bits, keeping last 8 bits same
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                | (VideoMemory[80*y+x] & 0xF000) >> 4
                                | (VideoMemory[80*y+x] & 0x00FF);

            x += buffer[1];
            // We dont want cursor to go out of the screen
            if(x >= 80) x = 79;
            if(x < 0) x = 0;

            y -= buffer[2];
            // We dont want cursor to go out of the screen
            if(y >= 25) y = 24;
            if(y < 0) y = 0;

            // Flip the foregroung and background colors of the character so that it looks like our cursor is present at that position
            // Interchanging foreground and background colors by swapping the first 4 bits and second 4 bits, keeping last 8 bits same
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                | (VideoMemory[80*y+x] & 0xF000) >> 4
                                | (VideoMemory[80*y+x] & 0x00FF);
        }
        /*
        // Check if buttons work or not
        for(uint8_t i = 0; i < 3; i++)
        {
            // If the current state is different from the previous state
            if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
            {
                if(buttons & (0x1<<i))
                    handler->OnMouseButtonReleased(i+1);
                else
                    handler->OnMouseButtonPressed(i+1);
            }
        }
        buttons = buffer[0];
        */
    }

    return esp;
}