#pragma once
#include <Core/Type/Types.h>

#include <Engine/UI/Widget/Widget.h>


namespace URay
{

class Label;
class VerticalLayout;

class ComponentSection final : public Widget
{
public:
    explicit ComponentSection(const String& title);

public:
    void Arrange(const Rect& rect) override;

    VerticalLayout* GetContent() const { return content; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;

private:
    Label* titleLabel = nullptr;
    VerticalLayout* content = nullptr;

    Rect headerRect = {};
};

} // namespace URay
