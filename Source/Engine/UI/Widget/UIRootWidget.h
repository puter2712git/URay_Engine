#pragma once

#include <Engine/UI/Widget/Layer/Layer.h>

#include <memory>

namespace URay
{

class UIRootWidget : public Widget
{
public:
    explicit UIRootWidget(std::unique_ptr<Widget> mainContent);

    template <typename T, typename... Args>
        requires std::derived_from<T, Layer>
    T* AddLayer(Args&&... args)
    {
        return AddChild<T>(std::forward<Args>(args)...);
    }

    void Arrange(const Rect& rect) override;

protected:
    std::optional<Rect> GetClipRect() const override { return rect; }
};

} // namespace URay
