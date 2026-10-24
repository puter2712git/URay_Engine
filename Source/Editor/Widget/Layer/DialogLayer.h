#pragma once

#include <Engine/UI/Layer/Layer.h>

namespace URay
{

class DialogLayer final : public Layer
{
public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

    template <typename T, typename... Args>
    T* OpenDialog(Args&&... args)
    {
        ClearChildren();
        activeDialog = AddChild<T>(std::forward<Args>(args)...);
        closeRequested = false;
        return static_cast<T*>(activeDialog);
    }

    void CloseDialog();

    bool HasDialog() const { return activeDialog != nullptr; }
    bool CanHit() const override { return activeDialog != nullptr; }

protected:
    void OnUpdate(float deltaTime) override;

private:
    Widget* activeDialog = nullptr;
    bool closeRequested = false;
};

} // namespace URay
