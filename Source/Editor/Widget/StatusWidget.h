#pragma once

#include "Editor/Widget/Panel/Panel.h"

namespace URay
{

class Label;

class StatusWidget final : public Panel
{
public:
    StatusWidget();
    ~StatusWidget() override;

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnDraw() override;
    void OnPaint(Render::UIDrawContext& context) override;

private:
    Label* statusLabel = nullptr;
    Label* fpsLabel = nullptr;
};

} // namespace URay
