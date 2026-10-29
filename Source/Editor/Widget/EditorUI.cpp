#include "EditorUI.h"

#include <Editor/Viewport/ViewportController.h>
#include <Editor/Widget/Docking/DockTabGroup.h>
#include <Editor/Widget/EditorRootWidget.h>
#include <Editor/Widget/Element/Splitter.h>
#include <Editor/Widget/Panel/Console/ConsolePanel.h>
#include <Editor/Widget/Panel/FileSystem/FileSystemPanel.h>
#include <Editor/Widget/Panel/Inspector/InspectorPanel.h>
#include <Editor/Widget/Panel/SceneTree/SceneTreePanel.h>
#include <Editor/Widget/Panel/StatusPanel.h>
#include <Editor/Widget/Panel/Viewport/ViewportPanel.h>
#include <Engine/Engine.h>
#include <Engine/UI/UISystem.h>

namespace URay
{

EditorUI::EditorUI(ViewportController& viewportController)
    : viewportController(viewportController)
{
}

EditorUI::~EditorUI() = default;

bool EditorUI::Initialize(UISystem& uiSystem)
{
    std::unique_ptr<Widget> root = CreateDefaultWidgets();
    rootWidget = static_cast<EditorRootWidget*>(root.get());
    uiSystem.SetRootWidget(std::move(root));

    return true;
}

PopupLayer& EditorUI::GetPopupLayer() const
{
    return rootWidget->GetPopupLayer();
}

DialogLayer& EditorUI::GetDialogLayer() const
{
    return rootWidget->GetDialogLayer();
}

std::unique_ptr<Widget> EditorUI::CreateDefaultWidgets()
{
    std::unique_ptr<SceneTreePanel> sceneTree = std::make_unique<SceneTreePanel>();
    std::unique_ptr<InspectorPanel> inspector = std::make_unique<InspectorPanel>();
    std::unique_ptr<ConsolePanel> console = std::make_unique<ConsolePanel>();
    std::unique_ptr<FileSystemPanel> filesystem = std::make_unique<FileSystemPanel>();
    std::unique_ptr<StatusPanel> status = std::make_unique<StatusPanel>();
    std::unique_ptr<ViewportPanel> viewport = std::make_unique<ViewportPanel>(viewportController);

    UISystem& uiSystem = gEngine->GetUISystem();
    auto makeTabGroup = [&uiSystem](const String& title, std::unique_ptr<Widget> panel)
    {
        auto group = std::make_unique<DockTabGroup>(uiSystem);
        group->AddTab(title, std::move(panel));
        return group;
    };

    auto sceneTreeGroup = makeTabGroup("Scene Tree", std::move(sceneTree));
    auto inspectorGroup = makeTabGroup("Inspector", std::move(inspector));
    auto statusGroup = makeTabGroup("Status", std::move(status));
    auto viewportGroup = makeTabGroup("Viewport", std::move(viewport));

    std::unique_ptr<Splitter> rightPanel2 = std::make_unique<Splitter>("RightPanel2", SplitAxis::Vertical, std::move(sceneTreeGroup), std::move(inspectorGroup));
    std::unique_ptr<Splitter> rightPanel = std::make_unique<Splitter>("RightPanel", SplitAxis::Vertical, std::move(statusGroup), std::move(rightPanel2));
    auto leftPanel2 = std::make_unique<DockTabGroup>(uiSystem);
    leftPanel2->AddTab("Console", std::move(console));
    leftPanel2->AddTab("File System", std::move(filesystem));
    std::unique_ptr<Splitter> leftPanel = std::make_unique<Splitter>("LeftPanel", SplitAxis::Vertical, std::move(viewportGroup), std::move(leftPanel2));

    auto mainContent = std::make_unique<Splitter>(
        "Root",
        SplitAxis::Horizontal,
        std::move(leftPanel),
        std::move(rightPanel));

    return std::make_unique<EditorRootWidget>(std::move(mainContent));
}

} // namespace URay
