#include "UISystem.h"

#include <Core/Input/InputSystem.h>
#include <Core/Math/Vector2.h>
#include <Engine/UI/Input/UIInputRouter.h>
#include <Engine/UI/Widget/Widget.h>
#include <Platform/Window/Window.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

UISystem::UISystem(Window& window, InputSystem& inputSystem)
    : window(window), inputSystem(inputSystem)
{
}

UISystem::~UISystem() = default;

void UISystem::Initialize()
{
    inputRouter = std::make_unique<UIInputRouter>(window);
    drawContext = std::make_unique<Render::UIDrawContext>();
}

void UISystem::Finalize()
{
    drawContext.reset();
    inputRouter.reset();
    root.reset();
}

void UISystem::SetRootWidget(std::unique_ptr<Widget> root)
{
    this->root = std::move(root);
}

void UISystem::ClearRootWidget()
{
    root.reset();
}

void UISystem::Update(float deltaTime)
{
    if (!root)
        return;

    inputRouter->Process(*root, inputSystem.GetSnapshot());
    root->Update(deltaTime);
}

void UISystem::PrepareRender()
{
    if (!root)
        return;

    const Extent2D clientSize = window.GetClientSize();

    Rect finalRect = {};
    finalRect.position = Vector2::Zero;
    finalRect.size = Vector2(
        static_cast<float>(clientSize.width),
        static_cast<float>(clientSize.height));

    root->Measure();
    root->Arrange(finalRect);
}

void UISystem::Paint()
{
    drawContext->Clear();

    if (!root)
        return;

    root->Paint(*drawContext);
}

bool UISystem::IsTextInputActive() const
{
    return inputRouter && root && inputRouter->IsTextInputActive(*root);
}

bool UISystem::TryHideWidget(Widget& widget)
{
    if (!root || !inputRouter)
        return false;

    if (!inputRouter->PrepareToHide(*root, widget))
        return false;

    widget.SetVisible(false);
    return true;
}

bool UISystem::PrepareForDetach(Widget& subtree)
{
    if (!root || !inputRouter)
        return false;

    const Widget* current = &subtree;
    while (current && current != root.get())
        current = current->GetParent();

    if (!current)
        return false;

    return inputRouter->PrepareToHide(*root, subtree);
}

Widget* UISystem::FindWidgetAt(const Vector2& position) const
{
    if (!root || !inputRouter)
        return nullptr;

    return inputRouter->FindTopWidget(*root, position);
}

} // namespace URay
