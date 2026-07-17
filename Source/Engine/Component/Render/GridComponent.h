#pragma once

#include "Engine/Component/Render/RenderComponent.h"

namespace URay
{

class GridComponent : public RenderComponent
{
public:
    DrawCommand SubmitCommand() override;
};

} // namespace URay
