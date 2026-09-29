#pragma once

#include "Editor/Widget/Widget.h"

#include "Core/Math/Color.h"
#include "Core/Math/Padding.h"

namespace URay
{

class Panel : public Widget
{
public:
    Panel();
    ~Panel() override;

public:
    const Color& GetBackgroundColor() const { return backgroundColor; }
    void SetBackgroundColor(const Color& color) { backgroundColor = color; }

    float GetOutlineWidth() const { return outlineWidth; }
    void SetOutlineWidth(float width) { outlineWidth = width; }

    const Color& GetOutlineColor() const { return outlineColor; }
    void SetOutlineColor(const Color& color) { outlineColor = color; }

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

protected:
    virtual void OnPaint(Render::UIDrawContext& context) override;

protected:
    Color backgroundColor = Color::White;

    float outlineWidth = 0.0f;
    Color outlineColor;

    Padding padding = {};
};

} // namespace URay
