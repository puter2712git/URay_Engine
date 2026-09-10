#pragma once

#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/Color.h"
#include "Core/Math/Vector3.h"

namespace URay::Render
{

struct PointLightObjectState
{
    Vector3 position = Vector3::Zero;
    float radius = 0.0f;
    float intensity = 0.0f;
    Color color = Color::White;
};

class PointLightObject : public RenderObject
{
public:
    PointLightObject(const PointLightObjectState& state);
    ~PointLightObject() override;

public:
    void Update(const PointLightObjectState& state);

private:
    Vector3 position = Vector3::Zero;
    float radius = 0.0f;
    float intensity = 0.0f;
    Color color = Color::White;
};

} // namespace URay::Render
