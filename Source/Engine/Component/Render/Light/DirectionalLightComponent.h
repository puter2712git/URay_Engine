#pragma once

#include "Engine/Component/Render/Light/LightComponent.h"

#include "Core/Math/Color.h"

namespace URay
{

class DirectionalLightComponent : public LightComponent
{
    URAY_CLASS(DirectionalLightComponent, LightComponent)

public:
    DirectionalLightComponent();
    ~DirectionalLightComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

protected:
    void UpdateRenderObject() override;
};

} // namespace URay
