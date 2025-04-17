#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "types.h"
#include "port.h"
#include "interrupts.h"
#include "driver.h"

class KeyboardEventHandler
{
public:
    KeyboardEventHandler();

    virtual void OnKeyDown(char);
    virtual void OnKeyUp(char);
};

class KeyboardDriver : public InterruptHandler, public Driver
{
    Port8Bit dataport;
    Port8Bit commandport;
    KeyboardEventHandler* handler;
public:
    KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler);
    ~KeyboardDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp); // A derived class can have its own implementation of a virtual function
    virtual void Activate();
};



#endif