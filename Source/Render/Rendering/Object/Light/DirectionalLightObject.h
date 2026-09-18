#pragma once

#include "Render/Rendering/Object/RenderObject.h"

#include "Core/Math/Color.h"
#include "Core/Math/Vector3.h"

namespace URay::Render
{

struct DirectionalLightObjectState
{
    Vector3 direction = Vector3::Forward;
    float intensity = 0.0f;
    Color color = Color::White;
    float bias = 0.0f;
};

class DirectionalLightObject : public RenderObject
{
public:
    DirectionalLightObject(const DirectionalLightObjectState& state);
    ~DirectionalLightObject() override;

public:
    void Update(const DirectionalLightObjectState& state);

public:
    const Vector3& GetDirection() const { return state.direction; }
    float GetIntensity() const { return state.intensity; }
    const Color& GetColor() const { return state.color; }
    float GetBias() const { return state.bias; }

private:
    DirectionalLightObjectState state;
};

} // namespace URay::Render
