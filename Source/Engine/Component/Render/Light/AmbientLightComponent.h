#pragma once

#include "Engine/Component/Render/Light/LightComponent.h"

#include "Core/Math/Color.h"

namespace URay
{

class AmbientLightComponent : public LightComponent
{
    URAY_CLASS(AmbientLightComponent, LightComponent)

public:
    AmbientLightComponent();
    ~AmbientLightComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

protected:
    void UpdateRenderObject() override;
};

} // namespace URay
