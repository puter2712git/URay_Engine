#pragma once

namespace URay
{

class Class;

class ClassRegistry
{
public:
    template <typename T>
    static void Register()
    {
        T::RegisterClass();
    }
};

} // namespace URay
