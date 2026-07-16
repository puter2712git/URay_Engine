#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Transform.h"

namespace URay
{

class Unit;

class Component
{
public:
    virtual void Update(float deltaTime);

    void UpdateWorldMatrix();

    Transform GetTransform() const
    {
        return transform;
    }

    Vector3 GetPosition() const
    {
        return transform.position;
    }
    void SetPosition(Vector3 newPosition)
    {
        transform.position = newPosition;
    }

    Vector3 GetRotation() const
    {
        return transform.rotation;
    }
    void SetRotation(Vector3 newRotation)
    {
        transform.rotation = newRotation;
    }

    Vector3 GetScale() const
    {
        return transform.scale;
    }
    void SetScale(Vector3 newScale)
    {
        transform.scale = newScale;
    }

    Matrix GetWorldMatrix() const
    {
        return worldMatrix;
    }

    Unit* GetOwner() const
    {
        return owner;
    }

protected:
    Transform transform = Transform::Identity;

    Matrix worldMatrix = Matrix::Identity;

    Unit* owner = nullptr;
};

} // namespace URay
