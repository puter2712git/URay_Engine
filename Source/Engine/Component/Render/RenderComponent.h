#pragma once

#include "Engine/Component/Component.h"

namespace URay
{

namespace Render
{
class RenderObject;
}

class RenderComponent : public Component
{
    URAY_CLASS(RenderComponent, Component)

public:
    virtual ~RenderComponent() override;

public:
    virtual void Update(float deltaTime) override;

    virtual void NotifyPropertyChanged(const Property& property);

    virtual Render::RenderObject* CreateRenderObject() = 0;

    void MarkDirty();

protected:
    virtual void UpdateRenderObject() = 0;

protected:
    Render::RenderObject* renderObject = nullptr;
};

} // namespace URay
