#pragma once

#include <Core/Type/Type.h>
#include <Engine/Object/Class/Class.h>
#include <Engine/Object/Class/ClassRegistry.h>
#include <Engine/Object/ObjectId.h>

#include <yaml-cpp/yaml.h>

namespace URay
{

class Class;

class Object
{
    URAY_OBJECT_ROOT_TYPE(Object)

    friend class ObjectSystem;

public:
    Object();
    virtual ~Object();

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(Object&&) = delete;

public:
    static void RegisterClass();

    static Class* StaticClass()
    {
        static Class cls(StaticType());
        return &cls;
    }

    virtual Class* GetClass() const
    {
        return Object::StaticClass();
    }

    virtual YAML::Node Serialize() const;
    virtual void Deserialize(const YAML::Node& node);

    virtual void NotifyPropertyChanged(const Property& property) {}

    bool IsA(const Type& type) const;
    bool IsA(Class* cls) const;

    template <typename T>
    bool IsA() const
    {
        return IsA(T::StaticClass());
    }

    ObjectId GetId() const { return id; }

private:
    ObjectId id = {};
};

} // namespace URay
