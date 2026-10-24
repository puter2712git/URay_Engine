#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Ray/EventRay.h>
#include <Engine/UI/Widget/Widget.h>

#include <vector>

namespace URay
{

class VirtualFileSystem;
struct VirtualFileEntry;

class FileGridView final : public Widget
{
public:
    void Arrange(const Rect& rect) override;

    const VirtualPath& GetDirectory() const { return directory; }
    void SetDirectory(VirtualFileSystem& fileSystem, const VirtualPath& path);

    EventRay<const VirtualFileEntry&>& GetOnEntryActivatedRay() { return onEntryActivated; }

protected:
    Vector2 OnMeasure() override;

private:
    void RebuildItems(const std::vector<VirtualFileEntry>& entries);

private:
    VirtualPath directory = {};
    EventRay<const VirtualFileEntry&> onEntryActivated;

    static constexpr float TileWidth = 96.0f;
    static constexpr float TileHeight = 112.0f;
    static constexpr float Gap = 8.0f;
    static constexpr uint32 ColumnCount = 4;
};

} // namespace URay
