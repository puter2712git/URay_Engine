#pragma once

#define URAY_CLASS(self, parent) \
public:                          \
    typedef parent Super;        \
                                 \
    static void RegisterClass(); \
    static Class* StaticClass();

#define URAY_REGISTER_CLASS(self)            \
    static struct URayRegister##self         \
    {                                        \
        URayRegister##self()                 \
        {                                    \
            ClassRegistry::Register<self>(); \
        }                                    \
    } _URayAutoRegister##self;

namespace URay
{

class Class;

class Object
{
public:
    static void RegisterClass();
    static Class* StaticClass();
};

} // namespace URay
