#ifndef __NILOS__DRIVERS__MOUSE_H__
#define __NILOS__DRIVERS__MOUSE_H__

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>

namespace nilos
{
    namespace drivers
    {
        class MouseEventHandler
        {
        public:
            MouseEventHandler();
        
            virtual void OnActivate();
            virtual void OnMouseDown(nilos::common::uint8_t button);
            virtual void OnMouseUp(nilos::common::uint8_t button);
            virtual void OnMouseMove(int x, int y);
        };
        
        class MouseDriver : public nilos::hardwarecommunication::InterruptHandler, public Driver
        {
            nilos::hardwarecommunication::Port8Bit dataport;
            nilos::hardwarecommunication::Port8Bit commandport;
        
            nilos::common::uint8_t buffer[3];
            nilos::common::uint8_t offset;
            nilos::common::uint8_t buttons;
        
            MouseEventHandler* handler;
        
        public:
            MouseDriver(nilos::hardwarecommunication::InterruptManager* manager, MouseEventHandler* handler);
            ~MouseDriver();
            virtual nilos::common::uint32_t HandleInterrupt(nilos::common::uint32_t esp); // A derived class can have its own implementation of a virtual function
            virtual void Activate();
        };

    }
}



#endif