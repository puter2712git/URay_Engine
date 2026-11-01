#include <Core/Type/TypeAlias.h>
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
    dockSystem = MakeUnique<DockSystem>(uiSystem, gEngine->GetObjectSystem());

    PanelContext context{viewportController};
    EditorLayout editorLayout;
    UniquePtr<Widget> content = editorLayout.LoadLayout(layout, uiSystem, context);
    UniquePtr<Widget> root;
    if (content)
        root = MakeUnique<EditorRootWidget>(std::move(content));
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

UniquePtr<Widget> EditorUI::CreateDefaultWidgets()
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

    auto sceneTreeGroup = MakeUnique<DockTabGroup>(uiSystem);
    auto inspectorGroup = MakeUnique<DockTabGroup>(uiSystem);
    auto statusGroup = MakeUnique<DockTabGroup>(uiSystem);
    auto viewportGroup = MakeUnique<DockTabGroup>(uiSystem);
    auto leftPanel2 = MakeUnique<DockTabGroup>(uiSystem);

    if (!addPanel(*sceneTreeGroup, String{SceneTreePanel::StaticType().name}) ||
        !addPanel(*inspectorGroup, String{InspectorPanel::StaticType().name}) ||
        !addPanel(*statusGroup, String{StatusPanel::StaticType().name}) ||
        !addPanel(*viewportGroup, String{ViewportPanel::StaticType().name}) ||
        !addPanel(*leftPanel2, String{ConsolePanel::StaticType().name}) ||
        !addPanel(*leftPanel2, String{FileSystemPanel::StaticType().name}))
        return nullptr;

    UniquePtr<Splitter> rightPanel2 = MakeUnique<Splitter>("RightPanel2", SplitAxis::Vertical, std::move(sceneTreeGroup), std::move(inspectorGroup));
    rightPanel2->SetSplitRatio(0.5f);
    UniquePtr<Splitter> rightPanel = MakeUnique<Splitter>("RightPanel", SplitAxis::Vertical, std::move(statusGroup), std::move(rightPanel2));
    UniquePtr<Splitter> leftPanel = MakeUnique<Splitter>("LeftPanel", SplitAxis::Vertical, std::move(viewportGroup), std::move(leftPanel2));

    rightPanel->SetSplitRatio(0.5f);
    leftPanel->SetSplitRatio(0.5f);

    auto mainContent = MakeUnique<Splitter>(
        "Root",
        SplitAxis::Horizontal,
        std::move(leftPanel),
        std::move(rightPanel));
    mainContent->SetSplitRatio(0.5f);

    return MakeUnique<EditorRootWidget>(std::move(mainContent));
}

} // namespace URay
