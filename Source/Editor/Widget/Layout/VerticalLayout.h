#pragma once

#include "Editor/Widget/Widget.h"

#include "Core/Math/Padding.h"
#include "Core/Type/Types.h"

namespace URay
{

enum class VerticalLayoutMode : uint8
{
    Top,
    Center,
    Bottom
};

class VerticalLayout final : public Widget
{
public:
    void Arrange(const Rect& rect) override;

    VerticalLayoutMode GetMode() const { return mode; }
    void SetMode(VerticalLayoutMode mode) { this->mode = mode; }

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

    float GetGap() const { return gap; }
    void SetGap(float gap) { this->gap = gap; }

private:
    void AlignTop();
    void AlignCenter();
    void AlignBottom();

private:
    VerticalLayoutMode mode = VerticalLayoutMode::Top;

    Padding padding = {};
    float gap = 0.0f;
};

} // namespace URay
