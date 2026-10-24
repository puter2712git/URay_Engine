#pragma once

#include <Engine/UI/Widget/UIRootWidget.h>

#include <memory>

namespace URay
{

class PopupLayer;
class DialogLayer;

class EditorRootWidget final : public UIRootWidget
{
public:
    explicit EditorRootWidget(std::unique_ptr<Widget> mainContent);

public:
    PopupLayer& GetPopupLayer() const { return *popupLayer; }
    DialogLayer& GetDialogLayer() const { return *dialogLayer; }

protected:
    std::optional<Rect> GetClipRect() const override { return rect; }

private:
    PopupLayer* popupLayer = nullptr;
    DialogLayer* dialogLayer = nullptr;
};

} // namespace URay
