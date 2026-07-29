#pragma once

#include "Engine/Object/Class/Class.h"
#include "Engine/Object/Class/ClassRegistry.h"

#include <yaml-cpp/yaml.h>

#define URAY_CLASS(self, parent)              \
public:                                       \
    typedef parent Super;                     \
                                              \
    static void RegisterClass();              \
    inline static Class* StaticClass()        \
    {                                         \
        static Class* cls = new Class(#self); \
        return cls;                           \
    }                                         \
    virtual Class* GetClass() const override  \
    {                                         \
        return self::StaticClass();           \
    }

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

    inline static Class* StaticClass()
    {
        static Class* cls = new Class("Object");
        return cls;
    }

    virtual Class* GetClass() const
    {
        return Object::StaticClass();
    }

    virtual YAML::Node Serialize();
    virtual void Deserialize(const YAML::Node& node);
};

} // namespace URay
