#pragma once

#include "Engine/Component/Component.h"

namespace URay
{

namespace RHI { class DrawCommandBuilder; }

class RenderComponent : public Component
{
    URAY_CLASS(RenderComponent, Component)

public:
    virtual void SubmitCommand(RHI::DrawCommandBuilder& builder) = 0;
};

} // namespace URay
