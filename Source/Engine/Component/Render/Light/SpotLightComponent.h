#pragma once

#include "Engine/Component/Render/Light/LightComponent.h"

namespace URay
{

class SpotLightComponent : public LightComponent
{
    URAY_CLASS(SpotLightComponent, LightComponent)

public:
    SpotLightComponent();
    ~SpotLightComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

    float GetRange() const { return range; }
    float GetInnerConeAngle() const { return innerConeAngle; }
    float GetOuterConeAngle() const { return outerConeAngle; }

protected:
    void UpdateRenderObject() override;

private:
    float range = 5.0f;

    float innerConeAngle = 20.0f;
    float outerConeAngle = 30.0f;
};

} // namespace URay
