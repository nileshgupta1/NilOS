#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "types.h"
#include "port.h"
#include "interrupts.h"

class KeyboardDriver : public InterruptHandler
{
    Port8Bit dataport;
    Port8Bit commandport;
public:
    KeyboardDriver(InterruptManager* manager);
    ~KeyboardDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp); // A derived class can have its own implementation of a virtual function
};



#endif