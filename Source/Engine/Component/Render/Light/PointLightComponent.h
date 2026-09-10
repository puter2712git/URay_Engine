#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/Math/Color.h"

namespace URay
{

class PointLightComponent : public RenderComponent
{
    URAY_CLASS(PointLightComponent, RenderComponent)

public:
    PointLightComponent();
    ~PointLightComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

protected:
    void UpdateRenderObject() override;

private:
    float radius = 3.0f;
    float intensity = 1.0f;
    Color color = Color::White;
};

} // namespace URay
