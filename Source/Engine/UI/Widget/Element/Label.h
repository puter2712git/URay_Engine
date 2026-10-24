#pragma once

#include <Engine/UI/Widget/Widget.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

#include <string>
#include <string_view>

namespace URay
{

enum class HorizontalTextAlignment : uint8
{
    Left,
    Center,
    Right,
};

enum class VerticalTextAlignment : uint8
{
    Top,
    Center,
    Bottom,
};

class Label final : public Widget
{
    URAY_TYPE(Label, Widget)

public:
    Label(std::string_view text = {});
    ~Label() override;

public:
    bool CanHit() const override { return false; }

    const std::string_view& GetText() const { return text; }
    void SetText(std::string_view text) { this->text = text; }

    void SetTextStyle(const Render::TextStyle& style) { this->style = style; }

    HorizontalTextAlignment GetHorizontalAlignment() const { return horizontalAlignment; }
    void SetHorizontalAlignment(HorizontalTextAlignment alignment) { horizontalAlignment = alignment; }

    VerticalTextAlignment GetVerticalAlignment() const { return verticalAlignment; }
    void SetVerticalAlignment(VerticalTextAlignment alignment) { verticalAlignment = alignment; }

protected:
    Vector2 OnMeasure() override;
    void OnPaint(Render::UIDrawContext& context) override;

private:
    std::string text;
    Render::TextStyle style = {};
    HorizontalTextAlignment horizontalAlignment = HorizontalTextAlignment::Left;
    VerticalTextAlignment verticalAlignment = VerticalTextAlignment::Top;
};

} // namespace URay
