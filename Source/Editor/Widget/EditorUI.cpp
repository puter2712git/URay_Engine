#include "EditorUI.h"

#include <Core/Log/LogSystem.h>
#include <Editor/Settings/EditorLayout.h>
#include <Editor/Viewport/ViewportController.h>
#include <Editor/Widget/Docking/DockSystem.h>
#include <Editor/Widget/Docking/DockTabGroup.h>
#include <Editor/Widget/EditorRootWidget.h>
#include <Editor/Widget/Element/Splitter.h>
#include <Editor/Widget/Panel/Console/ConsolePanel.h>
#include <Editor/Widget/Panel/FileSystem/FileSystemPanel.h>
#include <Editor/Widget/Panel/Inspector/InspectorPanel.h>
#include <Editor/Widget/Panel/PanelFactory.h>
#include <Editor/Widget/Panel/SceneTree/SceneTreePanel.h>
#include <Editor/Widget/Panel/StatusPanel.h>
#include <Editor/Widget/Panel/Viewport/ViewportPanel.h>
#include <Engine/Engine.h>
#include <Engine/Object/ObjectSystem.h>
#include <Engine/UI/UISystem.h>

namespace URay
{

EditorUI::EditorUI(ViewportController& viewportController)
    : viewportController(viewportController)
{
}

EditorUI::~EditorUI() = default;

bool EditorUI::Initialize(UISystem& uiSystem, const YAML::Node& layout)
{
    dockSystem = std::make_unique<DockSystem>(uiSystem, gEngine->GetObjectSystem());

    PanelContext context{viewportController};
    EditorLayout editorLayout;
    std::unique_ptr<Widget> content = editorLayout.LoadLayout(layout, uiSystem, context);
    std::unique_ptr<Widget> root;
    if (content)
        root = std::make_unique<EditorRootWidget>(std::move(content));
    else
    {
        if (layout && !layout.IsNull())
            URAY_LOG("[EditorUI] Invalid or unsupported layout; using default layout.");
        root = CreateDefaultWidgets();
    }
    if (!root)
        return false;
    rootWidget = static_cast<EditorRootWidget*>(root.get());
    uiSystem.SetRootWidget(std::move(root));

    return true;
}

bool EditorUI::SaveLayout(YAML::Node& outLayout) const
{
    const Widget* content = rootWidget ? rootWidget->GetMainContent() : nullptr;
    return content && EditorLayout{}.SaveLayout(*content, outLayout);
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
    UISystem& uiSystem = gEngine->GetUISystem();
    PanelContext context{viewportController};
    auto addPanel = [&context](DockTabGroup& group, const String& typeName)
    {
        const PanelRegistration* registration = PanelFactory::Find(typeName);
        auto panel = PanelFactory::Create(typeName, context);
        if (!registration || !panel)
        {
            URAY_LOG("[EditorUI] Failed to create panel: %s", typeName.c_str());
            return false;
        }
        group.AddTab(registration->title, std::move(panel));
        return true;
    };

    auto sceneTreeGroup = std::make_unique<DockTabGroup>(uiSystem);
    auto inspectorGroup = std::make_unique<DockTabGroup>(uiSystem);
    auto statusGroup = std::make_unique<DockTabGroup>(uiSystem);
    auto viewportGroup = std::make_unique<DockTabGroup>(uiSystem);
    auto leftPanel2 = std::make_unique<DockTabGroup>(uiSystem);

    if (!addPanel(*sceneTreeGroup, String{SceneTreePanel::StaticType().name}) ||
        !addPanel(*inspectorGroup, String{InspectorPanel::StaticType().name}) ||
        !addPanel(*statusGroup, String{StatusPanel::StaticType().name}) ||
        !addPanel(*viewportGroup, String{ViewportPanel::StaticType().name}) ||
        !addPanel(*leftPanel2, String{ConsolePanel::StaticType().name}) ||
        !addPanel(*leftPanel2, String{FileSystemPanel::StaticType().name}))
        return nullptr;

    std::unique_ptr<Splitter> rightPanel2 = std::make_unique<Splitter>("RightPanel2", SplitAxis::Vertical, std::move(sceneTreeGroup), std::move(inspectorGroup));
    rightPanel2->SetSplitRatio(0.5f);
    std::unique_ptr<Splitter> rightPanel = std::make_unique<Splitter>("RightPanel", SplitAxis::Vertical, std::move(statusGroup), std::move(rightPanel2));
    std::unique_ptr<Splitter> leftPanel = std::make_unique<Splitter>("LeftPanel", SplitAxis::Vertical, std::move(viewportGroup), std::move(leftPanel2));

    rightPanel->SetSplitRatio(0.5f);
    leftPanel->SetSplitRatio(0.5f);

    auto mainContent = std::make_unique<Splitter>(
        "Root",
        SplitAxis::Horizontal,
        std::move(leftPanel),
        std::move(rightPanel));
    mainContent->SetSplitRatio(0.5f);

    return std::make_unique<EditorRootWidget>(std::move(mainContent));
}

} // namespace URay
