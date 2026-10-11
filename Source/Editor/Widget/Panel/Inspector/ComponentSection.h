#pragma once

#include <Editor/Widget/Widget.h>

#include <string>

namespace URay
{

class Label;
class VerticalLayout;

class ComponentSection final : public Widget
{
public:
    explicit ComponentSection(const std::string& title);

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
