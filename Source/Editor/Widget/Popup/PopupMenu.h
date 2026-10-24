#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <functional>
#include <string_view>

namespace URay
{

class Button;
class Separator;
class Surface;
class VerticalLayout;

class PopupMenu : public Widget
{
public:
    PopupMenu();

public:
    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;

    std::optional<Rect> GetClipRect() const override { return rect; }

    Button* AddMenuItem(std::string_view text, std::function<void()> onClick);
    Separator* AddMenuSeparator();

private:
    Surface* surface = nullptr;
    VerticalLayout* layout = nullptr;
};

} // namespace URay
