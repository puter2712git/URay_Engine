#pragma once

#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Editor/Viewport/EditorView.h>

#include <optional>

namespace URay
{

struct FocusTransition
{
    Vector3 startPosition = Vector3::Zero;
    Vector3 targetPosition = Vector3::Zero;
    float elapsed = 0.0f;
    float duration = 0.5f;
};

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

    bool FocusAt(const Vector3& targetPosition, float distance);

    const EditorView& GetView() const { return view; }
    EditorView& GetView() { return view; }

private:
    EditorView view = {};

    float moveSpeed = 5.0f;
    float rotationSpeed = 0.1f;

    std::optional<FocusTransition> focusTransition = std::nullopt;
};

} // namespace URay
