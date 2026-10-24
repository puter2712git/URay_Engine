#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Panel/FileSystem/AssetEntry.h>
#include <Engine/UI/Widget/Widget.h>

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

    EventRay<const AssetEntry&>& GetOnSelectedRay() { return onSelected; }
    EventRay<const AssetEntry&>& GetOnActivatedRay() { return onActivated; }

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

protected:
    Vector2 OnMeasure() override;

    void OnUpdate(float deltaTime) override;

    std::optional<Rect> GetClipRect() const override { return rect; }

private:
    AssetEntry entry = {};

    bool selected = false;

    Surface* surface = nullptr;
    VerticalLayout* layout = nullptr;
    Surface* thumbnailSurface = nullptr;
    Image* thumbnailImage = nullptr;
    Label* nameLabel = nullptr;

    EventRay<const AssetEntry&> onSelected;
    EventRay<const AssetEntry&> onActivated;
};

} // namespace URay
