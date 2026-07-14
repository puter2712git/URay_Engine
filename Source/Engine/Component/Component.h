#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector3.h"

namespace URay
{

class Unit;

class Component
{
public:
    virtual void Update(float deltaTime);

    void UpdateWorldMatrix();

    Vector3 GetPosition() const
    {
        return position;
    }
    void SetPosition(Vector3 newPosition)
    {
        position = newPosition;
    }

    Vector3 GetRotation() const
    {
        return rotation;
    }
    void SetRotation(Vector3 newRotation)
    {
        rotation = newRotation;
    }

    Vector3 GetScale() const
    {
        return scale;
    }
    void SetScale(Vector3 newScale)
    {
        scale = newScale;
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
    Vector3 position = Vector3::Zero;
    Vector3 rotation = Vector3::Zero;
    Vector3 scale = Vector3::One;

    Matrix worldMatrix = Matrix::Identity;

    Unit* owner = nullptr;
};

} // namespace URay
