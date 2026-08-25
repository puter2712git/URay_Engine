#pragma once

#include "Engine/Component/RenderComponent.h"

namespace URay
{

namespace Render
{
class LineObject;
}

class GridComponent : public RenderComponent
{
    URAY_CLASS(GridComponent, RenderComponent)

public:
    Render::RenderObject* CreateRenderObject() override;

protected:
    void UpdateRenderObject() override;
};

} // namespace URay
