#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Widget.h>
#include <Engine/Asset/Asset.h>

namespace URay
{

class Class;
class VerticalLayout;

class AssetPickerPopup final : public Widget
{
public:
    explicit AssetPickerPopup(Class* assetClass, AssetHandle currentValue);

public:
    void Arrange(const Rect& rect) override;

    EventRay<AssetHandle>& GetValueSelectedRay() { return valueSelectedRay; }

protected:
    Vector2 OnMeasure() override;

private:
    VerticalLayout* layout = nullptr;

    EventRay<AssetHandle> valueSelectedRay;
};

} // namespace URay
