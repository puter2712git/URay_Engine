#pragma once

#include <Core/Type/TypeAlias.h>
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
    Vector<Property> GetAllProperties() const;

    void AddProperty(Property prop) { properties.push_back(prop); }
    const Vector<Property>& GetProperties() const { return properties; }

    const Type& GetType() const { return type; }

private:
    const Type& type;

    Vector<Property> properties;
};

#define URAY_OBJECT(self, parent)        \
    URAY_CLASS(self, parent)             \
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
