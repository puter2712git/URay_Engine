#pragma once

#include <Core/Type/TypeAlias.h>
#include <Core/Math/Vector2.h>
#include <Engine/Object/ObjectId.h>

#include <memory>
#include <unordered_map>

namespace URay::Render
{
class UIDrawContext;
}

namespace URay
{

class InputSystem;
class UIInputRouter;
class Widget;
class WindowWidget;
class Window;
struct WindowUIContext;

class UISystem
{
public:
    UISystem(Window& window, InputSystem& inputSystem);
    ~UISystem();

public:
    void Initialize();
    void Finalize();

    void SetRootWidget(UniquePtr<Widget> root);
    void ClearRootWidget();

    void Update(float deltaTime);
    void PrepareRender();
    void Paint();

    bool RegisterWindow(Window& window);
    bool UnregisterWindow(Window& window);

    bool SetAdditionalWindowRoot(Window& window, UniquePtr<Widget>& root);
    Widget* FindAdditionalWindowRoot(Window& window) const;
    Render::UIDrawContext* PrepareAdditionalWindowUI(Window& window);

    bool IsTextInputActive() const;

    bool TryHideWidget(Widget& widget);
    bool TryClearInputStateForWidget(Widget& widget);
    bool TryClearInputStateForSubtree(Widget& subtree);

    Widget* FindWidgetAt(const Vector2& position) const;
    Widget* FindWidgetAt(Window& targetWindow, const Vector2& position) const;

    Widget& GetRootWidget() const;
    Render::UIDrawContext& GetDrawContext() const;

    Widget* FindWindowRoot(Window& targetWindow) const;
    Window* FindWidgetWindow(const Widget& widget) const;

    ObjectId GetLastFocusedWidget(Window& targetWindow) const;
    void SetLastFocusedWidget(Window& targetWindow, ObjectId widgetId);

private:
    WindowUIContext* FindContext(Window& targetWindow) const;
    WindowUIContext* FindContext(const Widget& widget) const;

private:
    Window& window;
    InputSystem& inputSystem;

    UniquePtr<UIInputRouter> inputRouter = nullptr;

    UniquePtr<WindowUIContext> mainContext = nullptr;
    HashMap<Window*, UniquePtr<WindowUIContext>> windowContexts;

    Window* activeInputWindow = nullptr;
};

} // namespace URay
