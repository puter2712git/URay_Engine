#include "Component.h"

namespace URay
{

void Component::Update(float deltaTime)
{
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
