#pragma once

#include <Core/Math/Color.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector2.h>
#include <Core/Type/Types.h>

#include <cstdint>
#include <vector>

namespace URay
{

class Mesh;
class CameraComponent;

enum class GizmoHandleId
{
    None,
    X,
    Y,
    Z,
    XY,
    YZ,
    XZ,
    XYZ
};

enum class GizmoMode : uint8
{
    Translation,
    Rotation,
    Scale,

    Count
};

struct GizmoHandle
{
    GizmoHandleId id = GizmoHandleId::X;
    Mesh* mesh = nullptr;
    Matrix worldMatrix = Matrix::Identity;
    Color color = Color::White;
};

class GizmoStrategy
{
public:
    virtual ~GizmoStrategy() = default;

public:
    virtual GizmoMode GetMode() const = 0;
    virtual void UpdateHandles(const Matrix& gizmoWorld) = 0;
    virtual void StartDragging(const Vector2& position, GizmoHandleId handleId, const CameraComponent& camera) = 0;
    virtual void UpdateDragging(const Vector2& position, const CameraComponent& camera) = 0;
    virtual void EndDragging() = 0;

    const std::vector<GizmoHandle>& GetHandles() const { return handles; }
    std::vector<GizmoHandle>& GetHandles() { return handles; }

protected:
    std::vector<GizmoHandle> handles;
};

} // namespace URay
