#pragma once

#include <cstddef>

namespace URay
{

class Memory
{
public:
    static void* Allocate(std::size_t size);
    static void Free(void* ptr);
};

} // namespace URay
