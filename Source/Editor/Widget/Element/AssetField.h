#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Engine/Asset/AssetHandle.h>

#include <Core/Type/Ray/RayEmitter.h>

#include <memory>

namespace URay
{

class Class;
class TextBox;
class AssetPickerPopup;

class AssetField final : public Widget, public RayEmitter
{
public:
    explicit AssetField(Class* assetClass);
    ~AssetField() override;

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

    void SetValue(AssetHandle handle);
    AssetHandle GetValue() const { return value; }

protected:
    Vector2 OnMeasure() override;

private:
    void OnValueSelected(AssetHandle selectedHandle);

    TextBox* textBox = nullptr;

    Class* assetClass = nullptr;
    AssetHandle value = {};
    AssetPickerPopup* activePopup = nullptr;
    std::weak_ptr<void> popupLifetime;

};

} // namespace URay
