#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/Math/Color.h"

namespace URay
{

class AmbientLightComponent : public RenderComponent
{
    URAY_CLASS(AmbientLightComponent, RenderComponent)

public:
    AmbientLightComponent();
    ~AmbientLightComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

    const Color& GetColor() const { return color; }

protected:
    void UpdateRenderObject() override;

private:
    float intensity = 0.05f;
    Color color = Color::White;
};

} // namespace URay
