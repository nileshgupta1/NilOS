 
#ifndef __NILOS__MEMORYMANAGEMENT_H
#define __NILOS__MEMORYMANAGEMENT_H

#include <common/types.h>


namespace nilos
{
    
    struct MemoryChunk
    {
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        common::size_t size;
    };
    
    
    class MemoryManager
    {
        
    protected:
        MemoryChunk* first;
    public:
        
        static MemoryManager *activeMemoryManager;
        
        // the first parameter is the offset of the data that MemoryManager is going to handle. This is where we are going to put the first memory chunk
        // the second parameter is the size of the memory that we are going to handle. This is the size of the memory chunk
        MemoryManager(common::size_t first, common::size_t size);
        ~MemoryManager();
        
        void* malloc(common::size_t size);
        void free(void* ptr);
    };
}


void* operator new(unsigned size);
void* operator new[](unsigned size);

// placement new
void* operator new(unsigned size, void* ptr);
void* operator new[](unsigned size, void* ptr);

void operator delete(void* ptr);
void operator delete[](void* ptr);


#endif