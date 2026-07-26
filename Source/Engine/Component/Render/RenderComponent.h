#pragma once

#include "Engine/Component/Component.h"

namespace URay
{

class DrawCommandBuilder;

class RenderComponent : public Component
{
    URAY_CLASS(RenderComponent, Component)

public:
    virtual void SubmitCommand(DrawCommandBuilder& builder) = 0;
};

} // namespace URay
