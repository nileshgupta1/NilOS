#ifndef __NILOS__COMMON__TYPES_H
#define __NILOS__COMMON__TYPES_H

namespace nilos
{
    namespace common
    {

        typedef char                     int8_t;
        typedef unsigned char           uint8_t;
        typedef short                   int16_t;
        typedef unsigned short         uint16_t;
        typedef int                     int32_t;
        typedef unsigned int           uint32_t;
        typedef long long int           int64_t;
        typedef unsigned long long int uint64_t;
        // defining types ensures that all the data types have defined sizes which does not change with different compilers
        
    }
}


#endif