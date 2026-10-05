#pragma once

#include "Editor/Widget/Widget.h"

#include "Core/Math/Padding.h"

namespace URay
{

class HorizontalLayout final : public Widget
{
public:
    void Arrange(const Rect& rect) override;

    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding) { this->padding = padding; }

    float GetGap() const { return gap; }
    void SetGap(float gap) { this->gap = gap; }

private:
    Padding padding = {};
    float gap = 0.0f;
};

} // namespace URay
