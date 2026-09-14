#pragma once

#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/Color.h"
#include "Core/Math/Vector3.h"

namespace URay::Render
{

struct SpotLightObjectState
{
    Vector3 position = Vector3::Zero;
    Vector3 direction = Vector3::Forward;
    float range = 0.0f;
    float innerConeAngle = 0.0f;
    float outerConeAngle = 0.0f;
    float intensity = 0.0f;
    Color color = Color::White;
};

class SpotLightObject : public RenderObject
{
public:
    explicit SpotLightObject(const SpotLightObjectState& state);
    ~SpotLightObject() override;

public:
    void Update(const SpotLightObjectState& state);

    const Vector3& GetPosition() const { return state.position; }
    const Vector3& GetDirection() const { return state.direction; }
    float GetRange() const { return state.range; }
    float GetInnerConeAngle() const { return state.innerConeAngle; }
    float GetOuterConeAngle() const { return state.outerConeAngle; }
    float GetIntensity() const { return state.intensity; }
    const Color& GetColor() const { return state.color; }

private:
    SpotLightObjectState state;
};

} // namespace URay::Render
