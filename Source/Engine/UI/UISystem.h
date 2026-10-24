#pragma once

#include <memory>

namespace URay::Render
{
class UIDrawContext;
}

namespace URay
{

class InputSystem;
class UIInputRouter;
class Widget;
class Window;

class UISystem
{
public:
    UISystem(Window& window, InputSystem& inputSystem);
    ~UISystem();

    void Initialize();
    void Finalize();

    void SetRootWidget(std::unique_ptr<Widget> root);
    void ClearRootWidget();

    void Update(float deltaTime);
    void PrepareRender();
    void Paint();

    bool IsTextInputActive() const;

    Widget& GetRootWidget() const { return *root; }
    Render::UIDrawContext& GetDrawContext() const { return *drawContext; }

private:
    Window& window;
    InputSystem& inputSystem;

    std::unique_ptr<Widget> root = nullptr;
    std::unique_ptr<UIInputRouter> inputRouter = nullptr;
    std::unique_ptr<Render::UIDrawContext> drawContext = nullptr;
};

} // namespace URay
