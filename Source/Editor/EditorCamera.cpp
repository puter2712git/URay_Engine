#include "EditorCamera.h"

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
    if (!camera)
        return;

    Unit* cameraUnit = camera->GetOwner();
    if (!cameraUnit)
        return;

    TransformComponent* cameraTransform = cameraUnit->GetTransform();
    if (!cameraTransform)
        return;

    if (focusTransition)
    {
        FocusTransition& transition = *focusTransition;
        transition.elapsed += deltaTime;

        const float t = std::min(transition.elapsed / transition.duration, 1.0f);
        const float easedT = t * t * (3.0f - 2.0f * t);

        const Vector3 position = transition.startPosition + (transition.targetPosition - transition.startPosition) * easedT;

        cameraTransform->SetWorldPosition(position);

        if (t >= 1.0f)
            focusTransition.reset();

        return;
    }

    const Vector3 cameraPosition = cameraTransform->GetLocalPosition();
    const Vector3 moveDelta = (cameraTransform->TransformVectorNoScale(input.horizontalAxis) + input.verticalAxis) * moveSpeed * deltaTime;

    const Vector3 movedPosition = cameraPosition + moveDelta;
    cameraTransform->SetLocalPosition(movedPosition);

    Vector3 rotation = cameraTransform->GetLocalRotation();
    rotation.x -= input.mouseDelta.y * rotationSpeed;
    rotation.z -= input.mouseDelta.x * rotationSpeed;

    rotation.x = std::clamp(rotation.x, -89.0f, 89.0f);

    cameraTransform->SetLocalRotation(rotation);
}

bool EditorCamera::FocusAt(const Vector3& targetPosition, float distance)
{
    if (!camera || !camera->GetOwner())
        return false;

    TransformComponent* transform = camera->GetOwner()->GetTransform();
    if (!transform)
        return false;

    const Vector3 endPosition = targetPosition - transform->GetWorldForward() * distance;

    focusTransition = FocusTransition{
        .startPosition = transform->GetWorldPosition(),
        .targetPosition = endPosition,
        .duration = 0.5f
    };

    return true;
}

} // namespace URay
