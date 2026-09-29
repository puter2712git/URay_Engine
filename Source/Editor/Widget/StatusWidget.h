#pragma once

#include "Editor/Widget/Panel/Panel.h"

namespace URay
{

class Engine;

class StatusWidget final : public Panel
{
public:
    StatusWidget();
    ~StatusWidget() override;

protected:
    void OnDraw() override;
    void OnPaint(Render::UIDrawContext& context) override;
};

} // namespace URay
