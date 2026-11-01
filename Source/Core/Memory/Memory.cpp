#include "Memory.h"

namespace URay
{

void* Memory::Allocate(std::size_t size)
{
    return ::operator new(size);
}

void Memory::Free(void* ptr)
{
    ::operator delete(ptr);
}

} // namespace URay
