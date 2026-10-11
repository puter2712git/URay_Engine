#pragma once

#include <Editor/Widget/Widget.h>

#include <Engine/Asset/Asset.h>

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

    void SetValue(AssetHandle handle);
    AssetHandle GetValue() const { return value; }

    EventRay<AssetHandle>& GetValueChangedRay() { return valueChangedRay; }

protected:
    Vector2 OnMeasure() override;

private:
    TextBox* textBox = nullptr;

    Class* assetClass = nullptr;
    AssetHandle value = {};

    EventRay<AssetHandle> valueChangedRay;
};

} // namespace URay
