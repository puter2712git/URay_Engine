#pragma once

#include <Core/Type/Type.h>
#include <Engine/Object/Property/Property.h>

#include <vector>

namespace URay
{

class Class
{
public:
    Class(const Type& type);
    ~Class();

public:
    std::vector<Property> GetAllProperties() const;

    void AddProperty(Property prop) { properties.push_back(prop); }
    const std::vector<Property>& GetProperties() const { return properties; }

    const Type& GetType() const { return type; }

private:
    const Type& type;

    std::vector<Property> properties;
};

#define URAY_CLASS(self, parent)        \
    URAY_TYPE(self, parent)             \
public:                                 \
    static void RegisterClass();        \
                                        \
    static Class* StaticClass()         \
    {                                   \
        static Class cls(StaticType()); \
        return &cls;                    \
    }                                   \
                                        \
    Class* GetClass() const override    \
    {                                   \
        return self::StaticClass();     \
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
