#pragma once

#include "Engine/Object/Object.h"

#include <set>
#include <string>
#include <vector>

namespace URay
{

class Component;
class TransformComponent;

class Unit : public Object
{
    URAY_CLASS(Unit, Object)

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

    const std::set<Component*>& GetComponents() const
    {
        return components;
    }

    TransformComponent* GetTransform() const
    {
        return transform;
    }

    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }

    const std::vector<Unit*>& GetChildren() const { return children; }

private:
    std::string name;

    std::vector<Unit*> children;

    std::set<Component*> components;
    TransformComponent* transform = nullptr;
};

} // namespace URay
