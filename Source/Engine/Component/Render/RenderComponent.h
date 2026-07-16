#pragma once

#include "Engine/Component/Component.h"

#include "Render/DrawCommand.h"

namespace URay
{

class RenderComponent : public Component
{
public:
    virtual DrawCommand SubmitCommand() = 0;
};

} // namespace URay
