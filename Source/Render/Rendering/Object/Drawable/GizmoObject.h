#pragma once

#include <Render/Rendering/Object/Drawable/DrawableObject.h>

#include <Core/Math/Color.h>

#include <vector>

namespace URay
{
class Mesh;
class Material;
} // namespace URay

namespace URay::Render
{

struct GizmoObjectState
{
    struct Handle
    {
        Matrix worldMatrix = Matrix::Identity;
        Color colorTint = Color::White;
        Mesh* mesh = nullptr;
    };

    std::vector<Handle> handles;
    Material* material = nullptr;
};

class GizmoObject : public DrawableObject
{
    URAY_TYPE(GizmoObject, DrawableObject)

public:
    GizmoObject(const GizmoObjectState& state);
    ~GizmoObject() override;

public:
    void Update(const GizmoObjectState& state);
    void Submit(DrawCommandBuilder& builder, const DrawCommandParams& params) const override;

private:
    GizmoObjectState state;
};

} // namespace URay::Render
