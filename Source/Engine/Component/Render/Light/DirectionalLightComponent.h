#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/Math/Color.h"

namespace URay
{

class DirectionalLightComponent : public RenderComponent
{
    URAY_CLASS(DirectionalLightComponent, RenderComponent)

public:
    DirectionalLightComponent();
    ~DirectionalLightComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

    const Color& GetColor() const { return color; }

protected:
    void UpdateRenderObject() override;

private:
    float intensity = 1.0f;
    Color color = Color::White;
};

} // namespace URay
