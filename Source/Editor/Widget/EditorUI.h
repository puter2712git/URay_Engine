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
class DockSystem;

class EditorUI
{
public:
    explicit EditorUI(ViewportController& viewportController);
    ~EditorUI();

public:
    bool Initialize(UISystem& uiSystem);

    DockSystem& GetDockSystem() const { return *dockSystem; }
    PopupLayer& GetPopupLayer() const;
    DialogLayer& GetDialogLayer() const;

private:
    std::unique_ptr<Widget> CreateDefaultWidgets();

private:
    std::unique_ptr<DockSystem> dockSystem = nullptr;
    EditorRootWidget* rootWidget = nullptr;
    ViewportController& viewportController;
};

} // namespace URay
