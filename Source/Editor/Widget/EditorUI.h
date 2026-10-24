#pragma once

#include <memory>

namespace URay
{

class Widget;
class EditorRootWidget;
class ViewportController;
class PopupLayer;
class DialogLayer;
class UISystem;

class EditorUI
{
public:
    explicit EditorUI(ViewportController& viewportController);
    ~EditorUI();

public:
    bool Initialize(UISystem& uiSystem);
    PopupLayer& GetPopupLayer() const;
    DialogLayer& GetDialogLayer() const;

private:
    std::unique_ptr<Widget> CreateDefaultWidgets();

private:
    EditorRootWidget* rootWidget = nullptr;
    ViewportController& viewportController;
};

} // namespace URay
