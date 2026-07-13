#pragma once

#include "Engine/Component/Render/RenderComponent.h"

namespace URay
{

class BoxComponent : public RenderComponent
{
public:
    BoxComponent() = default;
    ~BoxComponent() = default;

public:
    virtual void SubmitCommand() override;

private:
};

} // namespace URay
