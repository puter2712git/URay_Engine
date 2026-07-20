#pragma once

#include "Engine/Component/Render/RenderComponent.h"

namespace URay
{

class GridComponent : public RenderComponent
{
public:
    void SubmitCommand(DrawCommandBuilder& builder) override;
};

} // namespace URay
