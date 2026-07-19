#pragma once

#include "Core/Math/Transform.h"
#include "Core/Math/Vector3.h"

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

    const std::set<Component*>& GetComponents() const
    {
        return components;
    }

    Transform GetTransform() const;
    void SetTransform(const Transform& inTransform);

    Vector3 GetPosition() const;
    void SetPosition(const Vector3& inPosition);

    Vector3 GetRotation() const;
    void SetRotation(const Vector3& inRotation);

    Vector3 GetScale() const;
    void SetScale(const Vector3& inScale);

private:
    std::set<Component*> components;
    Component* rootComponent = nullptr;
};

} // namespace URay
