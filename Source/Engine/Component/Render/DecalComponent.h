#pragma once

#include "Engine/Component/Render/RenderComponent.h"

#include "Core/Math/Vector3.h"

namespace URay
{

class Material;

class DecalComponent : public RenderComponent
{
    URAY_CLASS(DecalComponent, RenderComponent)

public:
    DecalComponent();
    ~DecalComponent();

public:
    Render::RenderObject* CreateRenderObject() override;

    const Vector3& GetExtent() const { return extent; }

protected:
    void UpdateRenderObject() override;

private:
    Vector3 extent = Vector3(0.5f, 0.5f, 0.5f);
    Material* material = nullptr;
};

} // namespace URay
