#pragma once

#include <set>

namespace URay
{

class Component;

class Unit
{
public:
    virtual void Update(float deltaTime);

    Component* AddComponent(Component* comp);

private:
    std::set<Component*> components;
};

} // namespace URay
