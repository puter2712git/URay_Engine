#pragma once

#include "Engine/Object/Class/Class.h"
#include "Engine/Object/Class/ClassRegistry.h"

#include <yaml-cpp/yaml.h>

#define URAY_CLASS(self, parent)                        \
public:                                                 \
    typedef parent Super;                               \
                                                        \
    static void RegisterClass();                        \
    inline static Class* StaticClass()                  \
    {                                                   \
        static Class cls(#self, parent::StaticClass()); \
        return &cls;                                    \
    }                                                   \
    virtual Class* GetClass() const override            \
    {                                                   \
        return self::StaticClass();                     \
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
    virtual ~Object() = default;

public:
    static void RegisterClass();

    inline static Class* StaticClass()
    {
        static Class cls("Object", nullptr);
        return &cls;
    }

    virtual Class* GetClass() const
    {
        return Object::StaticClass();
    }

    virtual YAML::Node Serialize() const;
    virtual void Deserialize(const YAML::Node& node);

    bool IsA(Class* cls) const;

    template <typename T>
    bool IsA() const
    {
        return IsA(T::StaticClass());
    }
};

template <typename To, typename From>
To* Cast(From* src)
{
    if (src == nullptr)
        return nullptr;

    if (src->IsA<To>())
    {
        return static_cast<To*>(src);
    }

    return nullptr;
}

} // namespace URay
