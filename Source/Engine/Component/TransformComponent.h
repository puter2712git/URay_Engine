#pragma once

#include <Engine/Component/Component.h>

#include <Core/Math/Matrix.h>
#include <Core/Math/Vector3.h>

namespace URay
{

class Unit;

class TransformComponent : public Component
{
    URAY_CLASS(TransformComponent, Component)

public:
    void NotifyPropertyChanged(const Property& property) override;

    void UpdateWorldMatrix();

    Vector3 TransformPoint(const Vector3& localPoint) const;
    Vector3 InvTransformPoint(const Vector3& worldPoint) const;

    Vector3 TransformVector(const Vector3& localVector) const;
    Vector3 InvTransformVector(const Vector3& worldVector) const;

    Vector3 TransformVectorNoScale(const Vector3& localVector) const;
    Vector3 InvTransformVectorNoScale(const Vector3& worldVector) const;

    Vector3 GetForward() const;
    Vector3 GetRight() const;
    Vector3 GetUp() const;

    const Vector3& GetLocalPosition() const { return position; }
    void SetLocalPosition(const Vector3& value)
    {
        position = value;
        SetDirty(true);
    }

    const Vector3& GetLocalRotation() const { return rotation; }
    void SetLocalRotation(const Vector3& inRotation)
    {
        rotation = inRotation;
        SetDirty(true);
    }

    const Vector3& GetLocalScale() const { return scale; }
    void SetLocalScale(const Vector3& inScale)
    {
        scale = inScale;
        SetDirty(true);
    }

    void SetDirty(bool dirty);

    const Matrix& GetWorldMatrix() const { return worldMatrix; }

    Vector3 GetWorldPosition() const;
    void SetWorldPosition(const Vector3& worldPosition);

    Vector3 GetWorldRotation() const;
    void SetWorldRotation(const Vector3& worldRotation);

    Vector3 GetWorldForward() const;
    Vector3 GetWorldRight() const;
    Vector3 GetWorldUp() const;

protected:
    void OnUpdate(float deltaTime) override;

    bool CanUpdateWhenUnloaded() const override { return true; }

private:
    TransformComponent* FindRelativeTransform(Unit* parentUnit) const;

private:
    Vector3 position = Vector3::Zero;
    Vector3 rotation = Vector3::Zero;
    Vector3 scale = Vector3::One;

    Matrix worldMatrix = Matrix::Identity;

    bool isDirty = false;
};

} // namespace URay
