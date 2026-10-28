#pragma once
#include <Core/Type/TypeAlias.h>

#include <Core/File/VirtualPath.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Engine/UI/Widget/Widget.h>

#include <optional>
#include <unordered_set>

namespace URay
{

class VirtualFileSystem;

class DirectoryTreeView final : public Widget, public RayEmitter
{
public:
    DirectoryTreeView(VirtualFileSystem& fileSystem, VirtualPath rootPath);

public:
    void Arrange(const Rect& rect) override;

    void SetSelectedPath(const VirtualPath& path);

protected:
    Vector2 OnMeasure() override;

    void OnUpdate(float deltaTime) override;

private:
    void RebuildItems();
    void AppendDirectory(const VirtualPath& path, uint32 depth);

    void ExpandAncestors(const VirtualPath& path);
    void OnExpandRequested(VirtualPath path);
    void OnItemActivated(VirtualPath path);

private:
    VirtualFileSystem& fileSystem;

    VirtualPath rootPath = {};
    VirtualPath selectedPath = {};

    std::unordered_set<String> expandedPaths;
    std::optional<VirtualPath> pendingExpandPath = std::nullopt;
    std::optional<VirtualPath> pendingSelectedPath = std::nullopt;

};

} // namespace URay
