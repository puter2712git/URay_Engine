#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/Math/Color.h"

namespace URay
{

class FogComponent : public RenderComponent
{
    URAY_CLASS(FogComponent, RenderComponent)

public:
    FogComponent();
    ~FogComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

protected:
    void UpdateRenderObject() override;

private:
    Color fogColor = Color::White;
    float fogStart = 1.0f;
    float fogEnd = 1000.0f;
    float density = 1.0f;
};

} // namespace URay
