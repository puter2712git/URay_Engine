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

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    Color backgroundColor = {};
    Padding padding = {};
};

} // namespace URay
