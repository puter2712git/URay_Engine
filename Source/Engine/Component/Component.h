#pragma once

namespace URay
{

class Unit;

class Component
{
public:
    virtual void Update(float deltaTime);

    Unit* GetOwner() const { return owner; }

private:
    Unit* owner = nullptr;
};

} // namespace URay
