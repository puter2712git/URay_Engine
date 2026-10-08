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

    const Vector3 cameraPosition = cameraTransform->GetPosition();
    const Vector3 moveDelta = (cameraTransform->TransformVectorNoScale(input.horizontalAxis) + input.verticalAxis) * moveSpeed * deltaTime;

    const Vector3 movedPosition = cameraPosition + moveDelta;
    cameraTransform->SetPosition(movedPosition);

    Vector3 rotation = cameraTransform->GetRotation();
    rotation.x -= input.mouseDelta.y * rotationSpeed;
    rotation.z -= input.mouseDelta.x * rotationSpeed;

    rotation.x = std::clamp(rotation.x, -89.0f, 89.0f);

    cameraTransform->SetRotation(rotation);
}

} // namespace URay
