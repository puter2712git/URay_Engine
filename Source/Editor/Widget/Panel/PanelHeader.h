#pragma once

#include <Engine/UI/Widget.h>

#include <string>

namespace URay
{

class HorizontalLayout;

class PanelHeader final : public Widget
{
public:
    PanelHeader(const std::string& name);

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    HorizontalLayout* layout = nullptr;
};

} // namespace URay
