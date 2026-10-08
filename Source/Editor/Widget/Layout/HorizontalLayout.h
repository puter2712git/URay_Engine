#pragma once

#include "Editor/Widget/Widget.h"

#include "Core/Math/Padding.h"
#include "Core/Type/Types.h"

namespace URay
{

enum class HorizontalLayoutMode : uint8
{
    Left,
    Center,
    Right
};

enum class CrossAxisAlignment : uint8
{
    Start,
    Center,
    End,
    Stretch
};

class HorizontalLayout final : public Widget
{
public:
    void Arrange(const Rect& rect) override;

    HorizontalLayoutMode GetMode() const { return mode; }
    void SetMode(HorizontalLayoutMode mode) { this->mode = mode; }

    CrossAxisAlignment GetCrossAxisAlignment() const { return crossAxisAlignment; }
    void SetCrossAxisAlignment(CrossAxisAlignment alignment) { crossAxisAlignment = alignment; }

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

    float GetGap() const { return gap; }
    void SetGap(float gap) { this->gap = gap; }

private:
    float CalculateStartOffsetX(const Rect& contentRect) const;
    float CalculateTotalWidth() const;

    float CalculateChildY(float childHeight, const Rect& contentRect) const;
    float CalculateChildHeight(float desiredHeight, const Rect& contentRect) const;

private:
    HorizontalLayoutMode mode = HorizontalLayoutMode::Left;
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::Start;

    Padding padding = {};
    float gap = 0.0f;
};

} // namespace URay
