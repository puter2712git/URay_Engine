#pragma once

#include <Editor/Widget/Style/EditorStyles.h>
#include <Editor/Widget/Widget.h>

#include <Core/Math/Padding.h>

#include <string>

namespace URay
{

class TextBox : public Widget
{
public:
    explicit TextBox(const std::string& text = {});

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

    Rect GetContentRect() const override;

    const std::string& GetText() const { return text; }
    void SetText(const std::string& text) { this->text = text; }

    void SetOutlineColor(const Color& color) { outlineColor = color; }
    void SetOutlineWidth(float width) { outlineWidth = width; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;

private:
    std::string text;

    Padding padding = Padding(6.0f, 4.0f);
    Render::TextStyle textStyle = EditorStyle::SmallText();

    Color outlineColor = EditorStyle::Border;
    float outlineWidth = 1.0f;
};

} // namespace URay
