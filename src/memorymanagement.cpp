 
#include <memorymanagement.h>

using namespace nilos;
using namespace nilos::common;


MemoryManager* MemoryManager::activeMemoryManager = 0;
        
MemoryManager::MemoryManager(size_t start_offset, size_t size)
{
    activeMemoryManager = this;
    
    if(size < sizeof(MemoryChunk))
    {
        first = 0;
    }
    else
    {
        first = (MemoryChunk*)start_offset;
        
        first -> allocated = false; // not yet allocated for any task
        first -> prev = 0;
        first -> next = 0;
        first -> size = size - sizeof(MemoryChunk);
    }
}

MemoryManager::~MemoryManager()
{
    if(activeMemoryManager == this)
        activeMemoryManager = 0;
}
   
// Iterate through the memory chunks and find a chunk that is not allocated and is big enough to hold the requested size
// If we find a chunk that is big enough, we split it into two chunks: one for the requested size and one for the remaining size
void* MemoryManager::malloc(size_t size)
{
    MemoryChunk *result = 0;
    
    for(MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk->next)
        if(chunk->size > size && !chunk->allocated)
            result = chunk;
        
    // If we didn't find a chunk that is big enough, and there is no paging, we cant't do anything, return 0
    if(result == 0)
        return 0;
    
    // If we found a chunk that is big enough, we split it into two chunks: one for the requested size and one for the remaining size
    if(result->size >= size + sizeof(MemoryChunk) + 1)
    {
        // Create new MemoryChunk (reprsenting the remaining memory) starting from the end of the requested chunk having the size of the remaining size
        MemoryChunk* temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
        
        temp->allocated = false;
        temp->size = result->size - size - sizeof(MemoryChunk);
        temp->prev = result;
        temp->next = result->next;
        if(temp->next != 0)
            temp->next->prev = temp;
        
        result->size = size;
        result->next = temp;
    }
    
    result->allocated = true;
    // Return the pointer to the data part after the MemoryChunk
    return (void*)(((size_t)result) + sizeof(MemoryChunk));
}

void MemoryManager::free(void* ptr)
{
    // We receive the pointer to the data part only in the parameter, so we need to get the pointer to the MemoryChunk
    MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    
    chunk -> allocated = false;
    
    // If left chunk is free
    if(chunk->prev != 0 && !chunk->prev->allocated)
    {
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        if(chunk->next != 0)
            chunk->next->prev = chunk->prev;
        
        chunk = chunk->prev;
    }
    
    // If right chunk is free
    if(chunk->next != 0 && !chunk->next->allocated)
    {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        if(chunk->next != 0)
            chunk->next->prev = chunk;
    }
    
}




void* operator new(unsigned size)
{
    if(MemoryManager::activeMemoryManager == 0)
        return 0;
    return MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new[](unsigned size)
{
    if(MemoryManager::activeMemoryManager == 0)
        return 0;
    return MemoryManager::activeMemoryManager->malloc(size);
}

void* operator new(unsigned size, void* ptr)
{
    return ptr;
}

void* operator new[](unsigned size, void* ptr)
{
    return ptr;
}

void operator delete(void* ptr)
{
    if(MemoryManager::activeMemoryManager != 0)
        MemoryManager::activeMemoryManager->free(ptr);
}

void operator delete[](void* ptr)
{
    if(MemoryManager::activeMemoryManager != 0)
        MemoryManager::activeMemoryManager->free(ptr);
}