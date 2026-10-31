#pragma once

#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class Window;

class WindowWidget final : public Widget
{
    URAY_CLASS(WindowWidget, Widget)

public:
    explicit WindowWidget(Window& window);

public:
    Window& GetWindow() const { return window; }

    void Arrange(const Rect& rect) override;

protected:
    std::optional<Rect> GetClipRect() const override { return rect; }

private:
    Window& window;
};

} // namespace URay
