#pragma once

#include <Editor/Widget/Widget.h>

#include <Core/Math/Color.h>
#include <Core/Math/Padding.h>

namespace URay
{

class Surface final : public Widget
{
public:
    bool CanHit() const override { return false; }

    void Arrange(const Rect& rect) override;
    Rect GetContentRect() const override;

    const Color& GetBackgroundColor() const { return backgroundColor; }
    void SetBackgroundColor(const Color& color) { backgroundColor = color; }

    const Color& GetOutlineColor() const { return outlineColor; }
    void SetOutlineColor(const Color& color) { outlineColor = color; }

    float GetOutlineWidth() const { return outlineWidth; }
    void SetOutlineWidth(float width) { outlineWidth = width; }

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

protected:
    Vector2 OnMeasure() override;

    void OnPaint(Render::UIDrawContext& context) override;

private:
    Color backgroundColor = {};
    Color outlineColor = Color::Black;
    float outlineWidth = 0.0f;
    Padding padding = {};
};

} // namespace URay
