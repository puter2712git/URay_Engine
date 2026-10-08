#pragma once

#include <Editor/Widget/Widget.h>

#include <Engine/Asset/Asset.h>

#include <Core/Math/Color.h>
#include <Core/Ray/EventRay.h>

#include <string_view>

namespace URay
{

class Label;

enum class ButtonState
{
    Normal,
    Hovering,
    Pressed
};

class Button final : public Widget
{
    URAY_TYPE(Button, Widget)

public:
    explicit Button(std::string_view text);
    explicit Button(AssetHandle textureHandle);
    ~Button() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    ButtonState GetState() const;

    EventRay<>& GetOnClickRay() { return onClickRay; }

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    Widget* content = nullptr;

    Color normalColor = Color(0.1f, 0.1f, 0.1f, 1.0f);
    Color hoverColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
    Color pressedColor = Color(0.2f, 0.2f, 0.2f, 1.0f);

    EventRay<> onClickRay;
};

} // namespace URay
