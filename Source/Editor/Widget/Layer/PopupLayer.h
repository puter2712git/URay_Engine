#pragma once

#include <Engine/UI/Widget/Layer/Layer.h>

namespace URay
{

class PopupLayer final : public Layer
{
public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

    template <typename T, typename... Args>
    T* OpenPopup(const Rect& anchorRect, Args&&... args)
    {
        ClearChildren();
        activePopup = nullptr;
        closeRequested = false;

        this->anchorRect = anchorRect;
        activePopup = AddChild<T>(std::forward<Args>(args)...);

        return static_cast<T*>(activePopup);
    }

    template <typename T, typename... Args>
    T* OpenPopup(const Vector2& position, Args&&... args)
    {
        const Rect anchorRect = {
            .position = position,
            .size = Vector2::Zero
        };

        return OpenPopup<T>(anchorRect, std::forward<Args>(args)...);
    }

    void ClosePopup();

    bool CanHit() const override
    {
        return activePopup != nullptr;
    }

protected:
    void OnUpdate(float deltaTime) override;

private:
    Widget* activePopup = nullptr;
    Rect anchorRect = {};

    bool closeRequested = false;
};

} // namespace URay
