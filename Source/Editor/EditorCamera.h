#pragma once

#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"

namespace URay
{

class CameraComponent;

struct EditorCameraInput
{
    Vector3 horizontalAxis = Vector3::Zero;
    Vector3 verticalAxis = Vector3::Zero;
    Vector2 mouseDelta = Vector2::Zero;
};

class EditorCamera
{
public:
    EditorCamera();
    ~EditorCamera();

public:
    void Update(float deltaTime, const EditorCameraInput& input);

    CameraComponent* GetCamera() const { return camera; }
    void SetCamera(CameraComponent* camera) { this->camera = camera; }

private:
    CameraComponent* camera = nullptr;

    float moveSpeed = 5.0f;
    float rotationSpeed = 0.1f;
};

} // namespace URay
