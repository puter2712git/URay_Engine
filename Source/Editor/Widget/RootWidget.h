#pragma once

#include <Editor/Widget/Widget.h>

#include <memory>

namespace URay
{

class PopupLayer;
class DialogLayer;

class RootWidget final : public Widget
{
public:
    explicit RootWidget(std::unique_ptr<Widget> mainContent);

public:
    void Arrange(const Rect& rect) override;

    PopupLayer& GetPopupLayer() const { return *popupLayer; }
    DialogLayer& GetDialogLayer() const { return *dialogLayer; }

protected:
    std::optional<Rect> GetClipRect() const override { return rect; }

private:
    Widget* mainContent = nullptr;
    PopupLayer* popupLayer = nullptr;
    DialogLayer* dialogLayer = nullptr;
};

} // namespace URay
