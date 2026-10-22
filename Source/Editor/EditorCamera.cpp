#include "EditorCamera.h"
#include <Core/Math/Math.h>

#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Scene/Unit.h>

#include <algorithm>

namespace URay
{

EditorCamera::EditorCamera() {}

EditorCamera::~EditorCamera() = default;

void EditorCamera::Update(float deltaTime, const EditorCameraInput& input)
{
    if (focusTransition)
    {
        FocusTransition& transition = *focusTransition;
        transition.elapsed += deltaTime;

        const float t = Math::Min(transition.elapsed / transition.duration, 1.0f);
        const float easedT = t * t * (3.0f - 2.0f * t);

        const Vector3 position = transition.startPosition + (transition.targetPosition - transition.startPosition) * easedT;

        view.position = position;

        if (t >= 1.0f)
            focusTransition.reset();

        return;
    }

    const Matrix rotationMatrix = Matrix::MakeRotation(view.rotation);

    const Vector3 cameraPosition = view.position;
    const Vector3 moveDelta = (rotationMatrix.TransformVector(input.horizontalAxis) + input.verticalAxis) * moveSpeed * deltaTime;

    const Vector3 movedPosition = cameraPosition + moveDelta;
    view.position = movedPosition;

    Vector3 rotation = view.rotation;
    rotation.x -= input.mouseDelta.y * rotationSpeed;
    rotation.z -= input.mouseDelta.x * rotationSpeed;

    rotation.x = Math::Clamp(rotation.x, -89.0f, 89.0f);

    view.rotation = rotation;
}

bool EditorCamera::FocusAt(const Vector3& targetPosition, float distance)
{
    const Vector3 endPosition = targetPosition - view.GetForward() * distance;

    focusTransition = FocusTransition{
        .startPosition = view.position,
        .targetPosition = endPosition,
        .duration = 0.5f
    };

    return true;
}

} // namespace URay
