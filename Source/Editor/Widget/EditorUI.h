#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>

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
    bool Initialize(UISystem& uiSystem, const YAML::Node& layout);
    bool SaveLayout(YAML::Node& outLayout) const;

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
