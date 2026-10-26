#pragma once
#include <Core/Type/TypeAlias.h>

#include <Engine/UI/Widget/Widget.h>


namespace URay
{

class HorizontalLayout;

class PanelHeader final : public Widget
{
public:
    PanelHeader(const String& name);

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    HorizontalLayout* layout = nullptr;
};

} // namespace URay
