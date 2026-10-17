#include "EditorView.h"

#include <Core/Math/Math.h>
#include <Core/Math/Vector4.h>

namespace URay
{

Vector3 EditorView::GetForward() const
{
    const Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    const Vector3 forward = rotationMatrix.TransformVector(Vector3::Forward);
    return forward;
}

Vector3 EditorView::GetUp() const
{
    const Matrix rotationMatrix = Matrix::MakeRotation(rotation);
    const Vector3 up = rotationMatrix.TransformVector(Vector3::Up);
    return up;
}

Matrix EditorView::GetViewMatrix() const
{
    const Vector3 forward = GetForward();
    const Vector3 up = GetUp();

    const Matrix viewMatrix = Matrix::MakeView(position, position + forward, up);

    return viewMatrix;
}

Matrix EditorView::GetProjMatrix(const Extent2D& extent) const
{
    uint32 width = extent.width;
    uint32 height = extent.height;

    if (width == 0 || height == 0)
        return Matrix::Identity;

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float fovRad = Math::DegToRad(fov);

    const Matrix projMatrix = Matrix::MakePerspective(fovRad, aspectRatio, nearPlane, farPlane);

    return projMatrix;
}

Ray EditorView::ScreenPointToRay(const Vector2& point, const Extent2D& extent) const
{
    uint32 width = extent.width;
    uint32 height = extent.height;

    if (width == 0 || height == 0)
        return {};

    const float ndcX = 2.0f * point.x / static_cast<float>(width) - 1.0f;
    const float ndcY = 1.0f - 2.0f * point.y / static_cast<float>(height);

    const Matrix inverseViewProj = (GetViewMatrix() * GetProjMatrix(extent)).Inverse();

    const auto unproject = [&](float depth)
    {
        const Vector4 clip(ndcX, ndcY, depth, 1.0f);
        const Vector4 worldH = clip * inverseViewProj;

        return Vector3(
            worldH.x / worldH.w,
            worldH.y / worldH.w,
            worldH.z / worldH.w);
    };

    const Vector3 nearPosition = unproject(0.0f);
    const Vector3 farPosition = unproject(1.0f);

    const Ray ray = {
        .origin = nearPosition,
        .direction = (farPosition - nearPosition).GetNormalized()
    };

    return ray;
}

} // namespace URay
