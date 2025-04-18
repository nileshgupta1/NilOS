#ifndef __NILOS__HARDWARECOMMUNICATION__PORT_H
#define __NILOS__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>

namespace nilos
{
    namespace hardwarecommunication
    {

        //Base Class for ports
        class Port{
        protected:  //Protected so that it cant be instantiated bc its purely virtual
            nilos::common::uint16_t portnumber;
            Port(nilos::common::uint16_t portnumber);
            ~Port();
        };
        
        class Port8Bit : public Port{
        public:
            Port8Bit(nilos::common::uint16_t portnumber);
            ~Port8Bit();
            virtual void Write(nilos::common::uint8_t data);  //Virtual function is a member function of a class that can be overridden by a derived class
            virtual nilos::common::uint8_t Read();
        protected:
            static inline nilos::common::uint8_t Read8(nilos::common::uint16_t _port)
            {
                nilos::common::uint8_t result;
                __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (_port));
                return result;
            }
        
            static inline void Write8(nilos::common::uint16_t _port, nilos::common::uint8_t _data)
            {
                __asm__ volatile("outb %0, %1" : : "a" (_data), "Nd" (_port));
            }
        };
        
        class Port8BitSlow : public Port8Bit{
        public:
            Port8BitSlow(nilos::common::uint16_t portnumber);
            ~Port8BitSlow();
            virtual void Write(nilos::common::uint8_t data);
            //---Inherits read method
        protected:
            static inline void Write8Slow(nilos::common::uint16_t _port, nilos::common::uint8_t _data)
            {
                __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (_data), "Nd" (_port));
                // added 2 garbage instructions behind to make the program wait a little until the port is done writing the data
            }
        };
        
        class Port16Bit : public Port{
        public:
            Port16Bit(nilos::common::uint16_t portnumber);
            ~Port16Bit();
            virtual void Write(nilos::common::uint16_t data);
            virtual nilos::common::uint16_t Read();
        protected:
            static inline nilos::common::uint16_t Read16(nilos::common::uint16_t _port)
            {
                nilos::common::uint16_t result;
                __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
                return result;
            }
        
            static inline void Write16(nilos::common::uint16_t _port, nilos::common::uint16_t _data)
            {
                __asm__ volatile("outw %0, %1" : : "a" (_data), "Nd" (_port));
            }
        };
        
        class Port32Bit : public Port{
        public:
            Port32Bit(nilos::common::uint16_t portnumber);
            ~Port32Bit();
            virtual void Write(nilos::common::uint32_t data);
            virtual nilos::common::uint32_t Read();
        protected:
            static inline nilos::common::uint32_t Read32(nilos::common::uint16_t _port)
            {
                nilos::common::uint32_t result;
                __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (_port));
                return result;
            }
        
            static inline void Write32(nilos::common::uint16_t _port, nilos::common::uint32_t _data)
            {
                __asm__ volatile("outl %0, %1" : : "a"(_data), "Nd" (_port));
            }
        };
    }
}


#endif