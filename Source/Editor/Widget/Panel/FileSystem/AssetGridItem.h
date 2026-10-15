#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Panel/FileSystem/AssetEntry.h>
#include <Editor/Widget/Widget.h>

namespace URay
{

class Surface;
class VerticalLayout;
class Label;
class Image;

class AssetGridItem final : public Widget
{
public:
    explicit AssetGridItem(AssetEntry entry);

public:
    void Arrange(const Rect& rect) override;

    void SetSelected(bool selected);

    const AssetEntry& GetEntry() const { return entry; }

    EventRay<const AssetEntry&>& GetOnSelectedRay() { return onSelectedRay; }
    EventRay<const AssetEntry&>& GetOnActivatedRay() { return onActivatedRay; }

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

protected:
    Vector2 OnMeasure() override;
    void OnUpdate(float deltaTime) override;

private:
    AssetEntry entry = {};

    bool selected = false;

    Surface* surface = nullptr;
    VerticalLayout* layout = nullptr;
    Surface* thumbnailSurface = nullptr;
    Image* thumbnailImage = nullptr;
    Label* nameLabel = nullptr;

    EventRay<const AssetEntry&> onSelectedRay;
    EventRay<const AssetEntry&> onActivatedRay;
};

} // namespace URay
