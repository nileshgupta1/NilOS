// 'include guards' prevent the header file from being included multiple times in a single translation unit
#ifndef __NILOS__GDT_H
#define __NILOS__GDT_H

#include <common/types.h>
 

//  Global Descriptor Table (GDT) segment structure:
//  +-------------------------------------------------+
//  |  Bit(s)  |  Byte(s)  |  Description             |
//  +-------------------------------------------------+
//  |  0-15   |  0-1          |  limit_lo (16 bits)   |
//  |  16-31  |  2-3          |  base_lo (16 bits)    |
//  |  32-39  |  4            |  base_hi (8 bits)     |
//  |  40-43  |  5 (bits 0-3) |  type (4 bits)        |
//  |  44-47  |  5 (bits 4-7) |  flags (4 bits)       |
//  |  48-51  |  6 (bits 0-3) |  limit_hi (4 bits)    |
//  |  52-55  |  6 (bits 4-7) |  base_vhi (4 bits)    |
//  |  56-63  |  7            |  reserved (8 bits)    |
//  +-------------------------------------------------+

//  Each entry of GDT will have 4 tables like this storing information about code, data, unused and null segments of a process:

//  +------------+----------------+--------------+---------------+-------------+--------------+-------------+--------------+
//  |  base_vhi  | flags |limit_hi|     type     |    base_hi   |           base_lo           |          limit_lo          |
//  +------------+----------------+--------------+--------------+---------------+-------------+-------------+--------------+
//  |  8 bits    |4 bits | 4 bits |    8 bits    |    8 bits    |           16 bits           |          16 bits           |
//  |  (56-63)   |(52-55)| (48-51)|    (40-47)   |    (32-39)   |           (16-31)           |          (0-15)            |
//  +------------+----------------+--------------+--------------+--------------+--------------+-------------+--------------+
//               7                6              5              4              3               2             1             0


namespace nilos
{

    class GlobalDescriptorTable{
    
        public:
            class SegmentDescriptor{
    
            private:
                nilos::common::uint16_t limit_lo; 
                nilos::common::uint16_t base_lo;       //Low Bytes of the pointer
                nilos::common::uint8_t base_hi;        //One byte extension for the pointer
                nilos::common::uint8_t type;           //Excess Bytes
                nilos::common::uint8_t limit_hi;
                nilos::common::uint8_t base_vhi;
    
            public:
                SegmentDescriptor(nilos::common::uint32_t base, nilos::common::uint32_t limit, nilos::common::uint8_t type); //Constructor
                //Return Pointer and the limit
                nilos::common::uint32_t Base();
                nilos::common::uint32_t Limit();
            } __attribute__((packed)); //Prevent Compiler from moving objects as it needs to be byte perfect
    
        private:
            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector; 
    
        public:
            GlobalDescriptorTable(); //Constructor
            ~GlobalDescriptorTable(); //Destructor
            
            nilos::common::uint16_t CodeSegmentSelector(); //Return offset of the code segment descriptor
            nilos::common::uint16_t DataSegmentSelector();
    };
}
 
#endif