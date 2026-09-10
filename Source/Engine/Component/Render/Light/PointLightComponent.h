#pragma once

#include "Engine/Component/Render/Light/LightComponent.h"

#include "Core/Math/Color.h"

namespace URay
{

class PointLightComponent : public LightComponent
{
    URAY_CLASS(PointLightComponent, LightComponent)

public:
    PointLightComponent();
    ~PointLightComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

protected:
    void UpdateRenderObject() override;

private:
    float radius = 3.0f;
};

} // namespace URay
