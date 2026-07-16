#include "Component.h"

namespace URay
{

void Component::Update(float deltaTime)
{
    UpdateWorldMatrix();
}

void Component::UpdateWorldMatrix()
{
    Matrix T = Matrix::MakeTranslation(transform.position);
    Matrix R = Matrix::MakeRotation(transform.rotation);
    Matrix S = Matrix::MakeScale(transform.scale);

    worldMatrix = S * R * T;
}

} // namespace URay
