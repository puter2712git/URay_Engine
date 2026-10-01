#include "WidgetSystem.h"

#include "Editor/Widget/Console/ConsoleWidget.h"
#include "Editor/Widget/Filesystem/FilesystemWidget.h"
#include "Editor/Widget/Input/UIInputRouter.h"
#include "Editor/Widget/InspectorWidget.h"
#include "Editor/Widget/SceneTreeWidget.h"
#include "Editor/Widget/Splitter.h"
#include "Editor/Widget/Panel/StatusPanel.h"
#include "Editor/Widget/Viewport/ViewportWidget.h"
#include "Editor/Widget/WidgetDrawer.h"

#include "Engine/Engine.h"

#include "Render/RenderSystem.h"
#include "Render/Rendering/Batch/UIDrawContext.h"
#include "Render/Rendering/RenderPass/UIPass.h"
#include "Render/Rendering/RenderPipeline.h"

#include <imgui/imgui.h>

#include <vector>

namespace URay
{

WidgetSystem::WidgetSystem() = default;

WidgetSystem::~WidgetSystem() = default;

bool WidgetSystem::Initialize()
{
    CreateDefaultWidgets();

    std::vector<Widget*> widgets = { root.get() };
    drawer = std::make_unique<WidgetDrawer>(widgets);

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::RenderPipeline& renderPipeline = renderSystem.GetPipeline();

    std::unique_ptr<Render::UIPass> uiPass = std::make_unique<Render::UIPass>(*drawer);
    renderPipeline.AddRenderPass(std::move(uiPass));

    inputRouter = std::make_unique<UIInputRouter>(gEngine->GetWindow());

    drawContext = std::make_unique<Render::UIDrawContext>();

    return true;
}

void WidgetSystem::Finalize()
{
    drawContext.reset();

    inputRouter.reset();

    drawer.reset();

    root.reset();
}

void WidgetSystem::Update(float deltaTime)
{
    inputRouter->Process(*root, gEngine->GetInputManager());

    root->Update(deltaTime);
}

void WidgetSystem::PrepareRender()
{
    ImGuiViewport* imGuiViewport = ImGui::GetMainViewport();
    root->Arrange(
        { .position = Vector2(imGuiViewport->WorkPos.x, imGuiViewport->WorkPos.y),
          .size = Vector2(imGuiViewport->WorkSize.x, imGuiViewport->WorkSize.y) });
}

void WidgetSystem::Paint()
{
    drawContext->Clear();

    root->Paint(*drawContext);
}

void WidgetSystem::CreateDefaultWidgets()
{
    std::unique_ptr<SceneTreeWidget> sceneTree = std::make_unique<SceneTreeWidget>();
    std::unique_ptr<InspectorWidget> inspector = std::make_unique<InspectorWidget>();
    std::unique_ptr<ConsoleWidget> console = std::make_unique<ConsoleWidget>();
    std::unique_ptr<FilesystemWidget> filesystem = std::make_unique<FilesystemWidget>();
    std::unique_ptr<StatusPanel> status = std::make_unique<StatusPanel>();
    std::unique_ptr<ViewportWidget> viewport = std::make_unique<ViewportWidget>();
    this->viewport = viewport.get();

    std::unique_ptr<Splitter> rightPanel2 = std::make_unique<Splitter>("RightPanel2", SplitAxis::Vertical, std::move(sceneTree), std::move(inspector));
    std::unique_ptr<Splitter> rightPanel = std::make_unique<Splitter>("RightPanel", SplitAxis::Vertical, std::move(status), std::move(rightPanel2));
    std::unique_ptr<Splitter> leftPanel2 = std::make_unique<Splitter>("LeftPanel2", SplitAxis::Horizontal, std::move(console), std::move(filesystem));
    std::unique_ptr<Splitter> leftPanel = std::make_unique<Splitter>("LeftPanel", SplitAxis::Vertical, std::move(viewport), std::move(leftPanel2));

    root = std::make_unique<Splitter>(
        "Root",
        SplitAxis::Horizontal,
        std::move(leftPanel),
        std::move(rightPanel));
}

} // namespace URay
