// 'include guards' prevent the header file from being included multiple times in a single translation unit
#ifndef __GDT_H
#define __GDT_H

#include "types.h"
 

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

class GlobalDescriptorTable{

    public:
        class SegmentDescriptor{

        private:
            uint16_t limit_lo; 
            uint16_t base_lo;       //Low Bytes of the pointer
            uint8_t base_hi;        //One byte extension for the pointer
            uint8_t type;           //Excess Bytes
            uint8_t flags_limit_hi;
            uint8_t base_vhi;

        public:
            SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type); //Constructor
            //Return Pointer and the limit
            uint32_t Base();
            uint32_t Limit();
        } __attribute__((packed)); //Prevent Compiler from moving objects as it needs to be byte perfect

    private:
        SegmentDescriptor nullSegmentSelector;
        SegmentDescriptor unusedSegmentSelector;
        SegmentDescriptor codeSegmentSelector;
        SegmentDescriptor dataSegmentSelector; 

    public:
        GlobalDescriptorTable(); //Constructor
        ~GlobalDescriptorTable(); //Destructor
        
        uint16_t CodeSegmentSelector(); //Return offset of the code segment descriptor
        uint16_t DataSegmentSelector();
};
 
#endif