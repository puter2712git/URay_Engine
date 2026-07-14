#include "Component.h"

namespace URay
{

void Component::Update(float deltaTime)
{
    Vector3 currPos = GetPosition();
    currPos.x += 1.0f * deltaTime;
    SetPosition(currPos);

    SetRotation(Vector3(45.0f, 30.0f, 0.0f));

    UpdateWorldMatrix();
}

void Component::UpdateWorldMatrix()
{
    Matrix T = Matrix::MakeTranslation(position);
    Matrix R = Matrix::MakeRotation(rotation);
    Matrix S = Matrix::MakeScale(scale);

    worldMatrix = S * R * T;
}

} // namespace URay
