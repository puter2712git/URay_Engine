#include <Editor/Widget/Panel/Viewport/ViewportPanel.h>

#include <Core/File/VirtualFileSystem.h>
#include <Core/Input/Action/InputActionSystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Type/Types.h>
#include <Editor/Editor.h>
#include <Editor/EditorCamera.h>
#include <Editor/Gizmo/TransformGizmo.h>
#include <Editor/Input/EditorInputActions.h>
#include <Editor/Pick/PickSystem.h>
#include <Editor/Play/PlaySession.h>
#include <Editor/Selection/SelectionSystem.h>
#include <Editor/Widget/Element/Button.h>
#include <Editor/Widget/Element/Image.h>
#include <Editor/Widget/Panel/Viewport/ViewportToolbar.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Component/Render/CameraComponent.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>
#include <Render/RHI/Texture/Sampler.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/Batch/UIDrawContext.h>
#include <Render/Rendering/RenderTargetRegistry.h>
#include <Render/Rendering/Renderer.h>
#include <Render/ResourceManager.h>

#include <Platform/Window/Window.h>

namespace URay
{

ViewportPanel::ViewportPanel()
    : renderer(gEngine->GetRenderSystem().GetRenderer()),
      selectionSystem(gEditor->GetSelectionSystem())
{
    SetBackgroundColor(EditorStyle::Surface);

    auto viewportToolbar = std::make_unique<ViewportToolbar>();
    toolbar = viewportToolbar.get();
    AddChild(std::move(viewportToolbar));

    gizmo = std::make_unique<TransformGizmo>();
    pickSystem = std::make_unique<PickSystem>(*gizmo);
    pickSystem->Initialize();

    editorCamera = std::make_unique<EditorCamera>();

    gEditor->GetSelectionSystem().GetOnActiveSelectionChangedRay().Register(this, [this](Unit*, Unit* unit)
                                                                            { gizmo->SetTarget(unit); });

    renderer.GetObjectIdReadbackRay().Register(this, [this](const Render::ObjectIdReadbackResult& result)
                                               { selectionSystem.SetSelection(pickSystem->FindTarget(result.objectId)); });

    auto gizmoButtons = toolbar->GetGizmoModeButtons();
    for (size_t i = 0; i < gizmoButtons.size(); ++i)
    {
        gizmoButtons[i]->GetOnClickRay().Register(
            this, [this, i]()
            { 
            GizmoMode mode = static_cast<GizmoMode>(i); 
            gizmo->SetMode(mode); });
    }

    auto gizmoSpaceToggleButton = toolbar->GetGizmoSpaceToggleButton();
    gizmoSpaceToggleButton->GetOnClickRay().Register(this,
                                                     [this, gizmoSpaceToggleButton]()
                                                     {
                                                         bool isLocal = gizmo->IsLocal();
                                                         bool gizmoSpace = !isLocal;
                                                         gizmo->SetIsLocal(gizmoSpace);

                                                         auto& content = gizmoSpaceToggleButton->GetChildren()[0];
                                                         Image* image = static_cast<Image*>(content.get());

                                                         if (image)
                                                         {
                                                             if (gizmo->IsLocal())
                                                             {
                                                                 image->SetTexture(EngineAsset::GizmoLocalSpaceIconTexture);
                                                             }
                                                             else
                                                             {
                                                                 image->SetTexture(EngineAsset::GizmoWorldSpaceIconTexture);
                                                             }
                                                         }
                                                     });
    bool gizmoIsLocal = gizmo->IsLocal();

    toolbar->GetPlayButton()->GetOnClickRay().Register(
        this,
        []()
        {
            gEditor->GetPlaySession().Start();
        });
    toolbar->GetStopButton()->GetOnClickRay().Register(
        this,
        []()
        {
            gEditor->GetPlaySession().Stop();
        });

    auto& content = gizmoSpaceToggleButton->GetChildren()[0];
    Image* image = static_cast<Image*>(content.get());

    if (gizmoIsLocal)
    {
        image->SetTexture(EngineAsset::GizmoLocalSpaceIconTexture);
    }
    else
    {
        image->SetTexture(EngineAsset::GizmoWorldSpaceIconTexture);
    }
}

ViewportPanel::~ViewportPanel()
{
    renderer.GetObjectIdReadbackRay().UnregisterAll(this);
    selectionSystem.GetOnActiveSelectionChangedRay().UnregisterAll(this);

    if (pickSystem)
    {
        pickSystem->Finalize();
        pickSystem.reset();
    }
    if (gizmo)
    {
        gizmo.reset();
        gizmo = nullptr;
    }
}

void ViewportPanel::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    constexpr float toolbarHeight = 40.0f;

    Rect toolbarRect = {};
    toolbarRect.position = Vector2(rect.position.x, rect.position.y);
    toolbarRect.size = Vector2(rect.size.x, toolbarHeight);

    toolbar->Arrange(toolbarRect);
}

EventReply ViewportPanel::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Right)
    {
        isCameraRotating = true;

        return EventReply{
            .requestFocus = true,
            .capturePointer = true,
        };
    }

    if (event.changedButton == MouseButton::Left)
    {
        const auto targetPosition = WindowToRenderTarget(event.position);
        if (!targetPosition)
            return {};

        const PickResult pickResult = pickSystem->Pick(editorCamera->GetCamera(), targetPosition->x, targetPosition->y);
        if (pickResult.isHit && pickResult.gizmoHandle != GizmoHandleId::None)
        {
            gizmo->StartDragging(*targetPosition, pickResult.gizmoHandle, *editorCamera->GetCamera());
        }
        else
        {
            renderer.GetObjectIdReadback().Request(
                static_cast<uint32>(targetPosition->x),
                static_cast<uint32>(targetPosition->y));
        }
    }

    return {
        .requestFocus = true,
        .capturePointer = true,
    };
}

EventReply ViewportPanel::OnPointerMove(const PointerEvent& event)
{
    if (isCameraRotating)
    {
        cameraMouseDelta.x += event.delta.x;
        cameraMouseDelta.y += event.delta.y;
        return {};
    }

    if (gizmo)
    {
        const auto targetPosition = WindowToRenderTarget(event.position);
        if (!targetPosition)
            return {};

        cachedPosition = *targetPosition;
    }

    return {};
}

EventReply ViewportPanel::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton == MouseButton::Right)
    {
        isCameraRotating = false;
    }

    if (event.changedButton == MouseButton::Left)
    {
        if (gizmo && gizmo->IsDragging())
        {
            gizmo->EndDragging();
        }
    }

    return EventReply{
        .releasePointer = event.pressedButtons == 0,
    };
}

void ViewportPanel::SetCamera(CameraComponent* camera)
{
    editorCamera->SetCamera(camera);
    camera->SetViewportExtent(GetTargetExtent());
}

void ViewportPanel::OnUpdate(float deltaTime)
{
    PickResult pickResult = pickSystem->Pick(editorCamera->GetCamera(), cachedPosition.x, cachedPosition.y);
    if (!pickResult.isHit || pickResult.gizmoHandle == GizmoHandleId::None)
    {
        gizmo->SetHoveredHandle(GizmoHandleId::None);
    }

    if (pickResult.gizmoHandle != GizmoHandleId::None)
    {
        gizmo->SetHoveredHandle(pickResult.gizmoHandle);
    }

    EditorCameraInput input = {};

    if (IsFocused())
    {
        const InputActionSnapshot& actions = gEngine->GetInputActionSystem().GetSnapshot();

        const auto wasPressed = [&actions](InputActionId id)
        {
            const InputActionValue* value = actions.FindAction(id);
            return value && value->pressed;
        };

        if (wasPressed(EditorInputAction::CycleGizmoMode))
        {
            CycleGizmoMode();
        }

        if (wasPressed(EditorInputAction::ToggleEditorCamera) &&
            gEditor->GetPlaySession().IsPlaying())
        {
            gEditor->SetUseEditorCamera(!gEditor->UsingEditorCamera());
        }

        if (const InputActionValue* move = actions.FindAction(EditorInputAction::ViewportMove))
        {
            input.horizontalAxis = Vector3(move->axis2D.x, move->axis2D.y, 0.0f);
        }

        if (const InputActionValue* elevate = actions.FindAction(EditorInputAction::ViewportElevate))
        {
            input.verticalAxis = Vector3(0.0f, 0.0f, elevate->axis1D);
        }
    }

    gizmo->Update(cachedPosition, *editorCamera->GetCamera());

    input.mouseDelta = cameraMouseDelta;

    editorCamera->Update(deltaTime, input);

    cameraMouseDelta = Vector2::Zero;
}

void ViewportPanel::OnPaint(Render::UIDrawContext& context)
{
    Panel::OnPaint(context);

    constexpr float toolbarHeight = 40.0f;

    imageRect = rect;
    imageRect.position.y += toolbarHeight;
    imageRect.size.y -= toolbarHeight;

    if (imageRect.size.x <= 0.0f || imageRect.size.y <= 0.0f)
        return;

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::Renderer& renderer = renderSystem.GetRenderer();

    Render::RenderTarget& postProcessRT =
        renderer.GetRenderTargetRegistry().Get(
            Render::RenderTargetId::PostProcess);

    Render::TextureView* view = postProcessRT.GetColorView(0);
    Render::Sampler* sampler =
        gEngine->GetRenderSystem()
            .GetResourceManager()
            .GetOrCreateSampler({});

    if (!view || !sampler)
        return;

    targetExtent = postProcessRT.GetExtent();

    Window& window = gEngine->GetWindow();
    const Extent2D clientSize = window.GetClientSize();
    const Extent2D framebufferSize = window.GetFramebufferSize();

    const float scaleX =
        clientSize.width > 0
            ? static_cast<float>(framebufferSize.width) / clientSize.width
            : 1.0f;

    const float scaleY =
        clientSize.height > 0
            ? static_cast<float>(framebufferSize.height) / clientSize.height
            : 1.0f;

    renderer.RequestSceneRenderTargetResize({
        .width = static_cast<uint32>(std::round(imageRect.size.x * scaleX)),
        .height = static_cast<uint32>(std::round(imageRect.size.y * scaleY)),
    });

    editorCamera->GetCamera()->SetViewportExtent(targetExtent);

    context.AddImage(imageRect, { .view = view,
                                  .sampler = sampler });
}

std::optional<Vector2> ViewportPanel::WindowToRenderTarget(const Vector2& windowPosition) const
{
    if (imageRect.size.x <= 0.0f || imageRect.size.y <= 0.0f)
        return std::nullopt;

    const Vector2 viewportPosition = Vector2(
        windowPosition.x - imageRect.position.x,
        windowPosition.y - imageRect.position.y);

    if (viewportPosition.x < 0.0f ||
        viewportPosition.y < 0.0f ||
        viewportPosition.x >= imageRect.size.x ||
        viewportPosition.y >= imageRect.size.y)
    {
        return std::nullopt;
    }

    if (targetExtent.width == 0 || targetExtent.height == 0)
        return std::nullopt;

    return Vector2(
        viewportPosition.x / imageRect.size.x * targetExtent.width,
        viewportPosition.y / imageRect.size.y * targetExtent.height);
}

void ViewportPanel::CycleGizmoMode()
{
    if (!gizmo)
        return;

    GizmoMode mode = gizmo->GetMode();
    const int modeIndex = static_cast<int>(mode);
    const GizmoMode newMode = static_cast<GizmoMode>(
        (modeIndex + 1) % static_cast<int>(GizmoMode::Count));

    gizmo->SetMode(newMode);
}

} // namespace URay
