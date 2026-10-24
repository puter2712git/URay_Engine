#pragma once

#include <Engine/UI/Widget/Widget.h>

#include <Engine/Asset/AssetHandle.h>

#include <Core/Ray/EventRay.h>

namespace URay
{

class Class;
class TextBox;

class AssetField final : public Widget
{
public:
    explicit AssetField(Class* assetClass);

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

    void SetValue(AssetHandle handle);
    AssetHandle GetValue() const { return value; }

    EventRay<AssetHandle>& GetValueChangedRay() { return onValueChanged; }

protected:
    Vector2 OnMeasure() override;

private:
    TextBox* textBox = nullptr;

    Class* assetClass = nullptr;
    AssetHandle value = {};

    EventRay<AssetHandle> onValueChanged;
};

} // namespace URay
