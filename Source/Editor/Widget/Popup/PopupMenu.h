#pragma once
#include <Core/Type/TypeAlias.h>

#include <Core/Type/Ray/RayEmitter.h>
#include <Engine/UI/Widget/Widget.h>

#include <memory>
#include <vector>

namespace URay
{

class Button;
class Separator;
class Surface;
class VerticalLayout;

class PopupMenu : public Widget, public RayEmitter
{
public:
    PopupMenu();
    ~PopupMenu() override;

public:
    void Arrange(const Rect& rect) override;

protected:
    Vector2 OnMeasure() override;

    std::optional<Rect> GetClipRect() const override { return rect; }

    Button* AddMenuItem(StringView text, int32 itemId);
    Separator* AddMenuSeparator();

private:
    struct MenuItem;
    Vector<UniquePtr<MenuItem>> menuItems;

    Surface* surface = nullptr;
    VerticalLayout* layout = nullptr;
};

} // namespace URay
