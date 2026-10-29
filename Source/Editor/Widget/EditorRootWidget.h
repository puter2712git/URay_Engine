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
    // Main content can be replaced by the docking system, so resolve it from the tree.
    Widget* GetMainContent() { return children.empty() ? nullptr : children.front().get(); }
    const Widget* GetMainContent() const { return children.empty() ? nullptr : children.front().get(); }
    PopupLayer& GetPopupLayer() const { return *popupLayer; }
    DialogLayer& GetDialogLayer() const { return *dialogLayer; }

protected:
    void OnPaint(Render::UIDrawContext& context) override;

    std::optional<Rect> GetClipRect() const override { return rect; }

private:
    PopupLayer* popupLayer = nullptr;
    DialogLayer* dialogLayer = nullptr;
};

} // namespace URay
