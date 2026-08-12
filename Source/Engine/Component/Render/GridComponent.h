#pragma once

#include "Engine/Component/Render/RenderComponent.h"

namespace URay
{

class GridComponent : public RenderComponent
{
    URAY_CLASS(GridComponent, RenderComponent)

public:
    void SubmitCommand(RHI::DrawCommandBuilder& builder) override;
};

} // namespace URay
