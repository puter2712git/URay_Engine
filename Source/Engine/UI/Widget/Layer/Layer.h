#pragma once

#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class Layer : public Widget
{
public:
    void Arrange(const Rect& rect) override;
};

} // namespace URay
