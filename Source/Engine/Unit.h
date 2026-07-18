#pragma once

#include <set>

namespace URay
{

class Component;

class Unit
{
public:
    Unit() = default;
    virtual ~Unit();

public:
    virtual void Update(float deltaTime);

    Component* AddComponent(Component* comp);

    template <typename T>
    T* GetComponent() const
    {
        for (Component* comp : components)
        {
            if (T* target = dynamic_cast<T*>(comp))
                return target;
        }
        return nullptr;
    }

private:
    std::set<Component*> components;
};

} // namespace URay
