#include "Layer.h"

namespace URay
{

void Layer::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    for (const auto& child : GetChildren())
    {
        child->Arrange(rect);
    }
}

} // namespace URay
