#pragma once

#include "Engine/Object/Object.h"
#include "Engine/Object/Property/Property.h"

#include <string>
#include <vector>

namespace URay
{

class Unit;

class Component : public Object
{
public:
    virtual ~Component() = default;

public:
    virtual void Update(float deltaTime);

    virtual void GetProperties(std::vector<Property>& properties);

    virtual const std::string& GetName() const { return name; }

    Unit* GetOwner() const
    {
        return owner;
    }
    void SetOwner(Unit* unit)
    {
        owner = unit;
    }

    bool IsEnabled() const
    {
        return enabled;
    }
    void SetEnabled(bool isEnabled)
    {
        enabled = isEnabled;
    }

protected:
    std::string name = "Component";

    Unit* owner = nullptr;

    bool enabled = true;
};

} // namespace URay
