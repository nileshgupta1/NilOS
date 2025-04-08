#ifndef __PORT_H
#define __PORT_H

#include "types.h"

    //Base Class for ports
    class Port{
    protected:  //Protected so that it cant be instantiated bc its purely virtual
        uint16_t portnumber;
        Port(uint16_t portnumber);
        ~Port();
    };

    class Port8Bit : public Port{
    public:
        Port8Bit(uint16_t portnumber);
        ~Port8Bit();
        virtual void Write(uint8_t data);  //Virtual function is a member function of a class that can be overridden by a derived class
        virtual uint8_t Read();
    };

    class Port8BitSlow : public Port8Bit{
    public:
        Port8BitSlow(uint16_t portnumber);
        ~Port8BitSlow();
        virtual void Write(uint8_t data);
        //---Inherits read method
    };

    class Port16Bit : public Port{
    public:
        Port16Bit(uint16_t portnumber);
        ~Port16Bit();
        virtual void Write(uint16_t data);
        virtual uint16_t Read();
    };

    class Port32Bit : public Port{
    public:
        Port32Bit(uint16_t portnumber);
        ~Port32Bit();
        virtual void Write(uint32_t data);
        virtual uint32_t Read();
    };

#endif