#pragma once

#include "Engine/Component/ComponentFactory.h"
#include "Engine/Object/Object.h"

#include <string>

namespace URay
{

class Unit;

class Component : public Object
{
    URAY_CLASS(Component, Object)

public:
    virtual ~Component() = default;

public:
    virtual void Update(float deltaTime);

    virtual void NotifyPropertyChanged(const Property& property);

    Unit* GetOwner() const { return owner; }
    void SetOwner(Unit* unit) { owner = unit; }

    bool IsEnabled() const { return enabled; }
    void SetEnabled(bool isEnabled) { enabled = isEnabled; }

protected:
    Unit* owner = nullptr;

    bool enabled = true;
};

#define URAY_REGISTER_COMPONENT(Type)                       \
    namespace                                               \
    {                                                       \
    const bool registered##Type = []()                      \
    {                                                       \
        ComponentFactory::RegisterComponent<Type>(          \
            std::string{ Type::StaticRuntimeType().name }); \
        return true;                                        \
    }();                                                    \
    }

} // namespace URay
