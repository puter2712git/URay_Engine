#pragma once

namespace URay
{

class Unit;

class Component
{
public:
    virtual ~Component() = default;

public:
    virtual void Update(float deltaTime);

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
    Unit* owner = nullptr;

    bool enabled = true;
};

} // namespace URay
