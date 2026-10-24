#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Ray/EventRay.h>
#include <Engine/UI/Widget.h>

#include <optional>
#include <unordered_set>

namespace URay
{

class VirtualFileSystem;

class DirectoryTreeView final : public Widget
{
public:
    DirectoryTreeView(VirtualFileSystem& fileSystem, VirtualPath rootPath);

public:
    void Arrange(const Rect& rect) override;

    void SetSelectedPath(const VirtualPath& path);

    EventRay<const VirtualPath&>& GetOnDirectoryActivatedRay() { return onDirectoryActivated; }

protected:
    Vector2 OnMeasure() override;

    void OnUpdate(float deltaTime) override;

private:
    void RebuildItems();
    void AppendDirectory(const VirtualPath& path, uint32 depth);

    void ExpandAncestors(const VirtualPath& path);

private:
    VirtualFileSystem& fileSystem;

    VirtualPath rootPath = {};
    VirtualPath selectedPath = {};

    std::unordered_set<std::string> expandedPaths;
    std::optional<VirtualPath> pendingExpandPath = std::nullopt;
    std::optional<VirtualPath> pendingSelectedPath = std::nullopt;

    EventRay<const VirtualPath&> onDirectoryActivated;
};

} // namespace URay
