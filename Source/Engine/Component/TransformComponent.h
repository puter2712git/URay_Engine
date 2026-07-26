#pragma once

#include "Engine/Component/Component.h"

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector3.h"

namespace URay
{

class TransformComponent : public Component
{
public:
    void Update(float deltaTime) override;

    virtual void GetProperties(std::vector<Property>& properties) override;

    virtual const std::string& GetName() const override { return name; }

    void UpdateWorldMatrix();

    Vector3 TransformPoint(const Vector3& point) const;
    Vector3 InvTransformPoint(const Vector3& worldPoint) const;

    Vector3 TransformVector(const Vector3& vector) const;
    Vector3 InvTransformVector(const Vector3& worldVector) const;

    Vector3 TransformVectorNoScale(const Vector3& vector) const;
    Vector3 InvTransformVectorNoScale(const Vector3& worldVector) const;

    Vector3 GetForward() const;
    Vector3 GetRight() const;
    Vector3 GetUp() const;

    const Vector3& GetPosition() const { return position; }
    void SetPosition(const Vector3& inPosition) { position = inPosition; }

    const Vector3& GetRotation() const { return rotation; }
    void SetRotation(const Vector3& inRotation) { rotation = inRotation; }

    const Vector3& GetScale() const { return scale; }
    void SetScale(const Vector3& inScale) { scale = inScale; }

    const Matrix& GetWorldMatrix() const { return worldMatrix; }

protected:
    std::string name = "Transform";

private:
    Vector3 position = Vector3::Zero;
    Vector3 rotation = Vector3::Zero;
    Vector3 scale = Vector3::One;

    Matrix worldMatrix = Matrix::Identity;
};

} // namespace URay
