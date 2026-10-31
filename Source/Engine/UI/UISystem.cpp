#include "UISystem.h"

#include <Core/Input/InputSystem.h>
#include <Core/Math/Vector2.h>
#include <Engine/UI/Input/UIInputRouter.h>
#include <Engine/UI/Widget/Widget.h>
#include <Engine/UI/Widget/WindowWidget.h>
#include <Platform/Window/Window.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

struct WindowUIContext
{
    std::unique_ptr<WindowWidget> windowWidget;
    Widget* root = nullptr;

    std::unique_ptr<UIInputRouter> inputRouter;
    std::unique_ptr<Render::UIDrawContext> drawContext;
};

UISystem::UISystem(Window& window, InputSystem& inputSystem)
    : window(window), inputSystem(inputSystem)
{
}

UISystem::~UISystem() = default;

void UISystem::Initialize()
{
    mainContext = std::make_unique<WindowUIContext>();

    mainContext->windowWidget = std::make_unique<WindowWidget>(window);
    mainContext->inputRouter = std::make_unique<UIInputRouter>(window);
    mainContext->drawContext = std::make_unique<Render::UIDrawContext>();
}

void UISystem::Finalize()
{
    windowContexts.clear();

    if (!mainContext)
        return;

    mainContext->drawContext.reset();
    mainContext->inputRouter.reset();
    mainContext->root = nullptr;
    mainContext->windowWidget.reset();

    mainContext.reset();
}

void UISystem::SetRootWidget(std::unique_ptr<Widget> newRoot)
{
    ClearRootWidget();

    if (!newRoot)
        return;

    mainContext->root = newRoot.get();
    mainContext->windowWidget->AddChild(std::move(newRoot));
}

void UISystem::ClearRootWidget()
{
    if (!mainContext)
        return;

    mainContext->root = nullptr;

    if (mainContext->windowWidget)
        mainContext->windowWidget->ClearChildren();
}

void UISystem::Update(float deltaTime)
{
    if (mainContext->root)
    {
        mainContext->inputRouter->Process(*mainContext->root, inputSystem.GetSnapshot());
        mainContext->root->Update(deltaTime);
    }

    for (auto& [window, context] : windowContexts)
    {
        if (!context->root)
            continue;

        WindowInputState* inputState = inputSystem.FindWindowState(*window);
        if (!inputState)
            continue;

        context->inputRouter->Process(*context->root, inputState->GetSnapshot());
        context->root->Update(deltaTime);
    }
}

void UISystem::PrepareRender()
{
    if (!mainContext->root)
        return;

    const Extent2D clientSize = window.GetClientSize();

    Rect finalRect = {};
    finalRect.position = Vector2::Zero;
    finalRect.size = Vector2(
        static_cast<float>(clientSize.width),
        static_cast<float>(clientSize.height));

    mainContext->windowWidget->Measure();
    mainContext->windowWidget->Arrange(finalRect);
}

void UISystem::Paint()
{
    mainContext->drawContext->Clear();

    if (!mainContext->root)
        return;

    mainContext->windowWidget->Paint(*mainContext->drawContext);
}

bool UISystem::RegisterWindow(Window& targetWindow)
{
    if (!mainContext || &targetWindow == &window || windowContexts.contains(&targetWindow))
        return false;

    if (!inputSystem.FindWindowState(targetWindow))
        return false;

    auto context = std::make_unique<WindowUIContext>();

    context->windowWidget = std::make_unique<WindowWidget>(targetWindow);
    context->inputRouter = std::make_unique<UIInputRouter>(targetWindow);
    context->drawContext = std::make_unique<Render::UIDrawContext>();

    windowContexts.emplace(&targetWindow, std::move(context));
    return true;
}

bool UISystem::UnregisterWindow(Window& targetWindow)
{
    return windowContexts.erase(&targetWindow) != 0;
}

bool UISystem::SetAdditionalWindowRoot(Window& window, std::unique_ptr<Widget>& root)
{
    const auto it = windowContexts.find(&window);

    if (it == windowContexts.end() || !root)
        return false;

    WindowUIContext& context = *it->second;

    if (context.root || root->GetParent())
        return false;

    Widget* rootPtr = root.get();

    context.windowWidget->AddChild(std::move(root));
    context.root = rootPtr;

    return true;
}

Widget* UISystem::FindAdditionalWindowRoot(Window& window) const
{
    const auto it = windowContexts.find(&window);

    if (it == windowContexts.end())
        return nullptr;

    return it->second->root;
}

Render::UIDrawContext* UISystem::PrepareAdditionalWindowUI(Window& window)
{
    const auto it = windowContexts.find(&window);
    if (it == windowContexts.end())
        return nullptr;

    WindowUIContext& context = *it->second;
    context.drawContext->Clear();

    if (!context.root)
        return context.drawContext.get();

    const Extent2D clientSize = window.GetClientSize();

    if (clientSize.width == 0 || clientSize.height == 0)
        return context.drawContext.get();

    Rect finalRect = {};
    finalRect.position = Vector2::Zero;
    finalRect.size = Vector2(static_cast<float>(clientSize.width), static_cast<float>(clientSize.height));

    context.windowWidget->Measure();
    context.windowWidget->Arrange(finalRect);
    context.windowWidget->Paint(*context.drawContext);

    return context.drawContext.get();
}

bool UISystem::IsTextInputActive() const
{
    return mainContext->inputRouter &&
           mainContext->root &&
           mainContext->inputRouter->IsTextInputActive(*mainContext->root);
}

bool UISystem::TryHideWidget(Widget& widget)
{
    if (!TryClearInputStateForSubtree(widget))
        return false;

    widget.SetVisible(false);
    return true;
}

bool UISystem::TryClearInputStateForWidget(Widget& widget)
{
    if (!mainContext->root || !mainContext->inputRouter)
        return false;

    return mainContext->inputRouter->TryClearInputStateForWidget(*mainContext->root, widget);
}

bool UISystem::TryClearInputStateForSubtree(Widget& subtree)
{
    if (!mainContext->root || !mainContext->inputRouter)
        return false;

    return mainContext->inputRouter->TryClearInputStateForSubtree(*mainContext->root, subtree);
}

Widget* UISystem::FindWidgetAt(const Vector2& position) const
{
    if (!mainContext->root || !mainContext->inputRouter)
        return nullptr;

    return mainContext->inputRouter->FindTopWidget(*mainContext->root, position);
}

Widget& UISystem::GetRootWidget() const
{
    return *mainContext->root;
}

Render::UIDrawContext& UISystem::GetDrawContext() const
{
    return *mainContext->drawContext;
}

} // namespace URay
