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

    ObjectId lastFocusedWidgetId = {};

    std::unique_ptr<Render::UIDrawContext> drawContext;
};

UISystem::UISystem(Window& window, InputSystem& inputSystem)
    : window(window), inputSystem(inputSystem)
{
}

UISystem::~UISystem() = default;

void UISystem::Initialize()
{
    inputRouter = std::make_unique<UIInputRouter>(*this);

    mainContext = std::make_unique<WindowUIContext>();

    mainContext->windowWidget = std::make_unique<WindowWidget>(window);
    mainContext->drawContext = std::make_unique<Render::UIDrawContext>();
}

void UISystem::Finalize()
{
    if (inputRouter)
    {
        inputRouter->CancelCaptureForWindow(window);
        inputRouter->ClearHoverForWindow(window);
        inputRouter->ClearFocusForWindow(window);

        for (const auto& [window, context] : windowContexts)
        {
            inputRouter->CancelCaptureForWindow(*window);
            inputRouter->ClearHoverForWindow(*window);
            inputRouter->ClearFocusForWindow(*window);
        }
    }

    activeInputWindow = nullptr;

    windowContexts.clear();

    if (!mainContext)
        return;

    mainContext->drawContext.reset();
    mainContext->root = nullptr;
    mainContext->windowWidget.reset();

    mainContext.reset();
    inputRouter.reset();
}

void UISystem::SetRootWidget(std::unique_ptr<Widget> newRoot)
{
    mainContext->lastFocusedWidgetId = {};

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

    if (inputRouter)
    {
        inputRouter->CancelCaptureForWindow(window);
        inputRouter->ClearHoverForWindow(window);
        inputRouter->ClearFocusForWindow(window);
    }

    mainContext->lastFocusedWidgetId = {};
    mainContext->root = nullptr;

    if (mainContext->windowWidget)
        mainContext->windowWidget->ClearChildren();
}

void UISystem::Update(float deltaTime)
{
    Window* nextActiveWindow = nullptr;

    if (inputSystem.GetSnapshot().IsWindowFocused())
    {
        nextActiveWindow = &window;
    }
    else
    {
        for (const auto& [targetWindow, context] : windowContexts)
        {
            WindowInputState* inputState = inputSystem.FindWindowState(*targetWindow);

            if (inputState && inputState->GetSnapshot().IsWindowFocused())
            {
                nextActiveWindow = targetWindow;
                break;
            }
        }
    }

    if (activeInputWindow != nextActiveWindow)
    {
        if (activeInputWindow)
        {
            inputRouter->CancelCaptureForWindow(*activeInputWindow);
            inputRouter->ClearHoverForWindow(*activeInputWindow);
            inputRouter->ClearFocusForWindow(*activeInputWindow);
        }

        activeInputWindow = nextActiveWindow;

        if (activeInputWindow)
            inputRouter->RestoreFocusForWindow(*activeInputWindow);
    }

    if (mainContext->root)
    {
        if (activeInputWindow == &window)
        {
            inputRouter->Process(window, *mainContext->root, inputSystem.GetSnapshot());
        }
        mainContext->root->Update(deltaTime);
    }

    for (auto& [targetWindow, context] : windowContexts)
    {
        if (!context->root)
            continue;

        WindowInputState* inputState = inputSystem.FindWindowState(*targetWindow);
        if (!inputState)
            continue;

        if (activeInputWindow == targetWindow)
        {
            inputRouter->Process(*targetWindow, *context->root, inputState->GetSnapshot());
        }
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
    context->drawContext = std::make_unique<Render::UIDrawContext>();

    windowContexts.emplace(&targetWindow, std::move(context));
    return true;
}

bool UISystem::UnregisterWindow(Window& targetWindow)
{
    const auto it = windowContexts.find(&targetWindow);
    if (it == windowContexts.end())
        return false;

    if (inputRouter)
    {
        inputRouter->CancelCaptureForWindow(targetWindow);
        inputRouter->ClearHoverForWindow(targetWindow);
        inputRouter->ClearFocusForWindow(targetWindow);
    }

    if (activeInputWindow == &targetWindow)
        activeInputWindow = nullptr;

    windowContexts.erase(it);
    return true;
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
    if (!inputRouter || !activeInputWindow)
        return false;

    Widget* root = FindWindowRoot(*activeInputWindow);
    return root && inputRouter->IsTextInputActive(*root);
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
    WindowUIContext* context = FindContext(widget);
    if (!context || !context->root)
        return false;

    return inputRouter->TryClearInputStateForWidget(context->windowWidget->GetWindow(), *context->root, widget);
}

bool UISystem::TryClearInputStateForSubtree(Widget& subtree)
{
    WindowUIContext* context = FindContext(subtree);
    if (!context || !context->root)
        return false;

    return inputRouter->TryClearInputStateForSubtree(context->windowWidget->GetWindow(), *context->root, subtree);
}

Widget* UISystem::FindWidgetAt(const Vector2& position) const
{
    return FindWidgetAt(window, position);
}

Widget* UISystem::FindWidgetAt(Window& targetWindow, const Vector2& position) const
{
    WindowUIContext* context = FindContext(targetWindow);
    if (!inputRouter || !context || !context->root)
        return nullptr;

    return inputRouter->FindTopWidget(*context->root, position);
}

Widget& UISystem::GetRootWidget() const
{
    return *mainContext->root;
}

Render::UIDrawContext& UISystem::GetDrawContext() const
{
    return *mainContext->drawContext;
}

Widget* UISystem::FindWindowRoot(Window& targetWindow) const
{
    WindowUIContext* context = FindContext(targetWindow);
    if (!context)
        return nullptr;

    return context->root;
}

Window* UISystem::FindWidgetWindow(const Widget& widget) const
{
    WindowUIContext* context = FindContext(widget);
    if (!context || !context->windowWidget)
        return nullptr;

    return &context->windowWidget->GetWindow();
}

ObjectId UISystem::GetLastFocusedWidget(Window& targetWindow) const
{
    WindowUIContext* context = FindContext(targetWindow);
    if (!context)
        return {};

    return context->lastFocusedWidgetId;
}

void UISystem::SetLastFocusedWidget(Window& targetWindow, ObjectId widgetId)
{
    WindowUIContext* context = FindContext(targetWindow);
    if (context)
        context->lastFocusedWidgetId = widgetId;
}

WindowUIContext* UISystem::FindContext(Window& targetWindow) const
{
    if (&targetWindow == &window)
        return mainContext.get();

    const auto it = windowContexts.find(&targetWindow);
    if (it == windowContexts.end())
        return nullptr;

    return it->second.get();
}

WindowUIContext* UISystem::FindContext(const Widget& widget) const
{
    for (const Widget* current = &widget; current; current = current->GetParent())
    {
        const auto* windowWidget = Cast<const WindowWidget>(current);
        if (!windowWidget)
            continue;

        WindowUIContext* context = FindContext(windowWidget->GetWindow());

        if (context && context->windowWidget.get() == windowWidget)
            return context;

        return nullptr;
    }

    return nullptr;
}

} // namespace URay
