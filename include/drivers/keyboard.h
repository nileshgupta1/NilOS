#ifndef __NILOS__DRIVERS__KEYBOARD_H__
#define __NILOS__DRIVERS__KEYBOARD_H__

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace nilos
{
    namespace drivers
    {

        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();
        
            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
        };
        
        class KeyboardDriver : public nilos::hardwarecommunication::InterruptHandler, public Driver
        {
            nilos::hardwarecommunication::Port8Bit dataport;
            nilos::hardwarecommunication::Port8Bit commandport;
            KeyboardEventHandler* handler;
        public:
            KeyboardDriver(nilos::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler* handler);
            ~KeyboardDriver();
            virtual nilos::common::uint32_t HandleInterrupt(nilos::common::uint32_t esp); // A derived class can have its own implementation of a virtual function
            virtual void Activate();
        };
    }
}




#endif