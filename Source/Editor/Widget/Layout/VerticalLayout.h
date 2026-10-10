#pragma once

#include <Editor/Widget/Layout/CrossAxisAlignment.h>
#include <Editor/Widget/Layout/MainAxisAlignment.h>
#include <Editor/Widget/Widget.h>

#include <Core/Math/Padding.h>
#include <Core/Type/Types.h>

namespace URay
{

class VerticalLayout final : public Widget
{
public:
    bool CanHit() const override { return false; }

    void Arrange(const Rect& rect) override;

    MainAxisAlignment GetMainAxisAlignment() const { return mainAxisAlignment; }
    void SetMainAxisAlignment(MainAxisAlignment alignment) { mainAxisAlignment = alignment; }

    CrossAxisAlignment GetCrossAxisAlignment() const { return crossAxisAlignment; }
    void SetCrossAxisAlignment(CrossAxisAlignment alignment) { crossAxisAlignment = alignment; }

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

    float GetGap() const { return gap; }
    void SetGap(float gap) { this->gap = gap; }

protected:
    Vector2 OnMeasure() override;

private:
    float CalculateStartOffsetY(const Rect& contentRect) const;
    float CalculateTotalHeight() const;

    float CalculateChildX(float childWidth, const Rect& contentRect) const;
    float CalculateChildWidth(const Widget& child, const Rect& contentRect) const;

    float GetChildLayoutWidth(const Widget& child) const;
    float GetChildLayoutHeight(const Widget& child) const;

private:
    MainAxisAlignment mainAxisAlignment = MainAxisAlignment::Start;
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::Start;

    Padding padding = {};
    float gap = 0.0f;
};

} // namespace URay
