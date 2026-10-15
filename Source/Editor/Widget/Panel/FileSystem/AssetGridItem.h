#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Panel/FileSystem/AssetEntry.h>
#include <Editor/Widget/Widget.h>

namespace URay
{

class AssetGridItem final : public Widget
{
public:
    explicit AssetGridItem(AssetEntry entry);

public:
    void SetSelected(bool selected);

    const AssetEntry& GetEntry() const { return entry; }

    EventRay<const AssetEntry&>& GetOnSelectedRay() { return onSelectedRay; }
    EventRay<const AssetEntry&>& GetOnActivatedRay() { return onActivatedRay; }

private:
    AssetEntry entry = {};

    EventRay<const AssetEntry&> onSelectedRay;
    EventRay<const AssetEntry&> onActivatedRay;
};

} // namespace URay
