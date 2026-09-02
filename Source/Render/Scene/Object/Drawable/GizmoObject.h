#pragma once

#include "Render/Scene/Object/Drawable/DrawableObject.h"

#include "Core/Math/Color.h"

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
    std::vector<Matrix> worldMatrices;
    Mesh* mesh = nullptr;
    Material* material = nullptr;
    std::vector<Color> colorTints;
};

class GizmoObject : public DrawableObject
{
public:
    GizmoObject(const GizmoObjectState& state);
    ~GizmoObject() override;

public:
    void Update(const GizmoObjectState& state);
    void Submit(DrawCommandBuilder& builder) const override;

private:
    std::vector<Matrix> worldMatrices;
    Mesh* mesh = nullptr;
    Material* material = nullptr;
    std::vector<Color> colorTints;
};

} // namespace URay::Render
