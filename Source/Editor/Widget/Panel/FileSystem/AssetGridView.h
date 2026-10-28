#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Editor/Widget/Panel/FileSystem/AssetEntry.h>
#include <Engine/UI/Widget/Widget.h>

#include <vector>

namespace URay
{

class AssetSystem;

class AssetGridView final : public Widget, public RayEmitter
{
public:
    AssetGridView();

    void Arrange(const Rect& rect) override;

    const VirtualPath& GetDirectory() const { return directory; }
    void SetDirectory(AssetSystem& assetSystem, const VirtualPath& path);

protected:
    Vector2 OnMeasure() override;

private:
    void RebuildItems(const std::vector<AssetEntry>& entries);
    void OnItemSelected(AssetEntry entry);
    void OnItemActivated(AssetEntry entry);

private:
    VirtualPath directory = {};

    static constexpr float TileWidth = 96.0f;
    static constexpr float TileHeight = 112.0f;
    static constexpr float Gap = 8.0f;
    static constexpr uint32 ColumnCount = 4;
};

} // namespace URay
