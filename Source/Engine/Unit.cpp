#include "Unit.h"

#include "Engine/Component/Component.h"

namespace URay
{

Unit::~Unit()
{
    for (Component* comp : components)
    {
        if (comp)
        {
            delete comp;
            comp = nullptr;
        }
    }

    components.clear();
}

void Unit::Update(float deltaTime)
{
    for (auto comp : components)
        comp->Update(deltaTime);
}

Component* Unit::AddComponent(Component* comp)
{
    if (!comp)
        return nullptr;

    if (!rootComponent)
    {
        rootComponent = comp;
    }

    components.insert(comp);
    return comp;
}

Transform Unit::GetTransform() const
{
    if (!rootComponent)
        return Transform::Identity;

    return rootComponent->GetTransform();
}

void Unit::SetTransform(const Transform& inTransform)
{
    if (!rootComponent)
        return;

    rootComponent->SetTransform(inTransform);
}

Vector3 Unit::GetPosition() const
{
    if (!rootComponent)
        return Vector3::Zero;

    return rootComponent->GetPosition();
}

void Unit::SetPosition(const Vector3& inPosition)
{
    if (!rootComponent)
        return;

    rootComponent->SetPosition(inPosition);
}

Vector3 Unit::GetRotation() const
{
    if (!rootComponent)
        return Vector3::Zero;

    return rootComponent->GetRotation();
}

void Unit::SetRotation(const Vector3& inRotation)
{
    if (!rootComponent)
        return;

    rootComponent->SetRotation(inRotation);
}

Vector3 Unit::GetScale() const
{
    if (!rootComponent)
        return Vector3::One;

    return rootComponent->GetScale();
}

void Unit::SetScale(const Vector3& inScale)
{
    if (!rootComponent)
        return;

    rootComponent->SetScale(inScale);
}

} // namespace URay
