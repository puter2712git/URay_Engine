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

protected:
    Unit* owner = nullptr;
};

} // namespace URay
