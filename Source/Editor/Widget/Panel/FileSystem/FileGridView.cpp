#include "FileGridView.h"

#include <Core/File/VirtualFileSystem.h>
#include <Editor/Widget/Panel/FileSystem/FileGridItem.h>

#include <algorithm>

namespace URay
{

void FileGridView::Arrange(const Rect& rect)
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

void FileGridView::SetDirectory(VirtualFileSystem& fileSystem, const VirtualPath& path)
{
    directory = path;

    std::vector<VirtualFileEntry> entries = fileSystem.ListDirectory(path);

    std::sort(entries.begin(), entries.end(),
              [](const VirtualFileEntry& lhs, const VirtualFileEntry& rhs)
              {
                  if (lhs.isDirectory != rhs.isDirectory)
                      return lhs.isDirectory > rhs.isDirectory;

                  return lhs.path.GetFilename() < rhs.path.GetFilename();
              });

    ClearChildren();

    for (const VirtualFileEntry& entry : entries)
    {
        FileGridItem* item = AddChild<FileGridItem>(entry);

        item->GetOnActivatedRay().Register(
            this,
            [this](const VirtualFileEntry& activatedRay)
            {
                entryActivatedRay.Emit(activatedRay);
            });
    }
}

Vector2 FileGridView::OnMeasure()
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
