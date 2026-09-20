#pragma once

#include "Render/Rendering/Object/RenderObject.h"

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
    float bias = 0.0f;
};

class PointLightObject : public RenderObject
{
public:
    PointLightObject(const PointLightObjectState& state);
    ~PointLightObject() override;

public:
    void Update(const PointLightObjectState& state);

    const Vector3& GetPosition() const { return state.position; }
    float GetRadius() const { return state.radius; }
    float GetIntensity() const { return state.intensity; }
    const Color& GetColor() const { return state.color; }
    float GetBias() const { return state.bias; }

private:
    PointLightObjectState state;
};

} // namespace URay::Render
