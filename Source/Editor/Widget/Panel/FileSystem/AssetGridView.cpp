#include "AssetGridView.h"

#include <Core/File/VirtualFileSystem.h>
#include <Editor/Widget/Panel/FileSystem/AssetEntry.h>
#include <Editor/Widget/Panel/FileSystem/AssetGridItem.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Metadata/AssetMetadataSerializer.h>

#include <algorithm>

namespace URay
{

void AssetGridView::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    for (size_t index = 0; index < children.size(); ++index)
    {
        const uint32 column = static_cast<uint32>(index) % ColumnCount;
        const uint32 row = static_cast<uint32>(index) / ColumnCount;

        Rect itemRect;
        itemRect.position = rect.position + Vector2(column * (TileWidth + Gap), row * (TileHeight + Gap));
        itemRect.size = Vector2(TileWidth, TileHeight);

        children[index]->Arrange(itemRect);
    }
}

void AssetGridView::SetDirectory(AssetSystem& assetSystem, const VirtualPath& path)
{
    directory = path;

    VirtualFileSystem& fileSystem = assetSystem.GetFileSystem();
    const std::vector<VirtualFileEntry> fileEntries = fileSystem.ListDirectory(path);
    std::vector<AssetEntry> assetEntries;

    for (const VirtualFileEntry& entry : fileEntries)
    {
        if (entry.isDirectory)
        {
            assetEntries.push_back(AssetEntry{
                .kind = AssetEntryKind::Directory,
                .path = entry.path,
                .displayName = entry.path.GetFilename() });
            continue;
        }

        if (entry.path.GetExtension() == ".scene")
        {
            assetEntries.push_back(AssetEntry{
                .kind = AssetEntryKind::Scene,
                .path = entry.path,
                .displayName = entry.path.GetStem() });
            continue;
        }

        if (entry.path.GetExtension() == ".meta")
            continue;

        AssetMetadata metadata;

        const VirtualPath metadataPath = assetSystem.GetMetadataPath(entry.path);
        if (!assetSystem.GetMetadataSerializer().Load(metadata, metadataPath))
            continue;

        if (metadata.assetPath.ToString() != entry.path.ToString())
            continue;

        assetEntries.push_back(AssetEntry{
            .kind = AssetEntryKind::Asset,
            .path = metadata.assetPath,
            .displayName = metadata.assetPath.GetStem(),
            .assetHandle = metadata.handle,
            .className = metadata.className });
    }

    std::sort(assetEntries.begin(), assetEntries.end(),
              [](const AssetEntry& lhs, const AssetEntry& rhs)
              {
                  if (lhs.kind != rhs.kind)
                      return lhs.kind == AssetEntryKind::Directory;

                  return lhs.displayName < rhs.displayName;
              });

    RebuildItems(assetEntries);
}

void AssetGridView::RebuildItems(const std::vector<AssetEntry>& entries)
{
    ClearChildren();

    for (const AssetEntry& entry : entries)
    {
        AssetGridItem* item = AddChild<AssetGridItem>(entry);

        item->GetOnSelectedRay().Register(
            this,
            [this](const AssetEntry& selectedEntry)
            {
                for (const auto& child : children)
                {
                    AssetGridItem* gridItem = static_cast<AssetGridItem*>(child.get());
                    const bool isSelected =
                        gridItem->GetEntry().path.ToString() == selectedEntry.path.ToString();
                    gridItem->SetSelected(isSelected);
                }

                entrySelectedRay.Emit(selectedEntry);
            });

        item->GetOnActivatedRay().Register(
            this,
            [this](const AssetEntry& activatedEntry)
            {
                entryActivatedRay.Emit(activatedEntry);
            });
    }
}

Vector2 AssetGridView::OnMeasure()
{
    const uint32 count = static_cast<uint32>(children.size());
    const uint32 rowCount = (count + ColumnCount - 1) / ColumnCount;

    const float width = ColumnCount * TileWidth + (ColumnCount - 1) * Gap;
    const float height = rowCount == 0
                             ? 0.0f
                             : rowCount * TileHeight + (rowCount - 1) * Gap;

    return Vector2(width, height);
}

} // namespace URay
