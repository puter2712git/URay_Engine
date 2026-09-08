#pragma once

#include "Render/Scene/Object/RenderObject.h"

#include "Core/Math/Color.h"

namespace URay::Render
{

struct AmbientLightObjectState
{
    float intensity = 0.05f;
    Color color = Color::White;
};

class AmbientLightObject : public RenderObject
{
public:
    AmbientLightObject(const AmbientLightObjectState& state);
    ~AmbientLightObject() override;

public:
    void Update(const AmbientLightObjectState& state);

    float GetIntensity() const { return intensity; }
    const Color& GetColor() const { return color; }

private:
    float intensity = 0.0f;
    Color color = Color::White;
};

} // namespace URay::Render
