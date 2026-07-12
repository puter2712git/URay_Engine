#pragma once

#include "Engine/Component/Component.h"

namespace URay
{

class RenderComponent : public Component
{
public:
    virtual void SubmitCommand();
};

} // namespace URay
