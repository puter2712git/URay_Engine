#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/Math/Color.h"

namespace URay
{

class LightComponent : public RenderComponent
{
    URAY_CLASS(LightComponent, RenderComponent)

public:
    virtual ~LightComponent() override = default;

public:
    float GetIntensity() const { return intensity; }
    const Color& GetColor() const { return color; }

protected:
    float intensity = 1.0f;
    Color color = Color::White;
};

} // namespace URay
