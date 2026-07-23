#pragma once

#include "Engine/Component/Render/RenderComponent.h"

namespace URay
{

class SpriteComponent : public RenderComponent
{
public:
    SpriteComponent();
    virtual ~SpriteComponent() override;
};

} // namespace URay
