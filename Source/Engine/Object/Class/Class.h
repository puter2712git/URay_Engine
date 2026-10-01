#pragma once

#include "Engine/Object/Property/Property.h"

#include "Core/Type/RuntimeType.h"

#include <string>
#include <vector>

namespace URay
{

class Class
{
public:
    Class(const RuntimeType& type);
    ~Class();

public:
    std::vector<Property> GetAllProperties() const;

    void AddProperty(Property prop) { properties.push_back(prop); }
    const std::vector<Property>& GetProperties() const { return properties; }

    const RuntimeType& GetType() const { return type; }

private:
    const RuntimeType& type;

    std::vector<Property> properties;
};

#define URAY_CLASS(self, parent)               \
    URAY_TYPE(self, parent)                    \
public:                                        \
    static void RegisterClass();               \
                                               \
    static Class* StaticClass()                \
    {                                          \
        static Class cls(StaticRuntimeType()); \
        return &cls;                           \
    }                                          \
                                               \
    Class* GetClass() const override           \
    {                                          \
        return self::StaticClass();            \
    }

#define URAY_REGISTER_CLASS(self)                                \
    static struct URayRegister##self                             \
    {                                                            \
        URayRegister##self()                                     \
        {                                                        \
            self::RegisterClass();                               \
            ClassRegistry::Get().Register(*self::StaticClass()); \
        }                                                        \
    } _URayAutoRegister##self;

} // namespace URay
