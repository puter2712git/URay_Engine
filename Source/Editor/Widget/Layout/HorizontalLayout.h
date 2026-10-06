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

class HorizontalLayout final : public Widget
{
public:
    void Arrange(const Rect& rect) override;

    HorizontalLayoutMode GetMode() const { return mode; }
    void SetMode(HorizontalLayoutMode mode) { this->mode = mode; }

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

    float GetGap() const { return gap; }
    void SetGap(float gap) { this->gap = gap; }

private:
    void AlignLeft();
    void AlignCenter();
    void AlignRight();

private:
    HorizontalLayoutMode mode = HorizontalLayoutMode::Left;

    Padding padding = {};
    float gap = 0.0f;
};

} // namespace URay
