#pragma once

#include "Engine/Component/Component.h"

namespace URay
{

class DrawCommandBuilder;

class RenderComponent : public Component
{
public:
    virtual void SubmitCommand(DrawCommandBuilder& builder) = 0;
};

} // namespace URay
