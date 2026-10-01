#pragma once

#include "Engine/Object/Class/Class.h"
#include "Engine/Object/Class/ClassRegistry.h"

#include "Core/Type/RuntimeType.h"

#include <yaml-cpp/yaml.h>

namespace URay
{

class Class;

class Object
{
    URAY_ROOT_TYPE(Object)

public:
    virtual ~Object() = default;

public:
    static void RegisterClass();

    static Class* StaticClass()
    {
        static Class cls(StaticRuntimeType());
        return &cls;
    }

    virtual Class* GetClass() const
    {
        return Object::StaticClass();
    }

    virtual YAML::Node Serialize() const;
    virtual void Deserialize(const YAML::Node& node);

    virtual void NotifyPropertyChanged(const Property& property) {}

    bool IsA(Class* cls) const;

    template <typename T>
    bool IsA() const
    {
        return IsA(T::StaticClass());
    }
};

} // namespace URay
