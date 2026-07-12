#pragma once

namespace URay
{

class Unit;

class Component
{
public:
    virtual void Update(float deltaTime);

private:
    Unit* owner = nullptr;
};

} // namespace URay
