#pragma once

#include <Editor/Widget/Widget.h>

namespace URay
{

class PopupLayer final : public Widget
{
public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

    template <typename T, typename... Args>
    T* OpenPopup(const Rect& anchorRect, Args&&... args)
    {
        ClosePopup();

        this->anchorRect = anchorRect;
        activePopup = AddChild<T>(std::forward<Args>(args)...);

        return static_cast<T*>(activePopup);
    }

    void ClosePopup();

    bool CanHit() const override { return activePopup != nullptr; }

private:
    Widget* activePopup = nullptr;
    Rect anchorRect = {};
};

} // namespace URay
