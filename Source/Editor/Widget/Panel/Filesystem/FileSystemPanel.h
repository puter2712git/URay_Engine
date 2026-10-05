#pragma once

#include "Editor/Widget/Panel/Panel.h"

#include "Core/File/VirtualPath.h"

#include <vector>

namespace URay
{

class Engine;
class VirtualFileSystem;

struct FileEntry
{
    VirtualPath path = {};
    bool isDirectory = false;
};

class FileSystemPanel final : public Panel
{
    URAY_TYPE(FileSystemPanel, Panel)

public:
    FileSystemPanel();
    ~FileSystemPanel();

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnDraw() override;

private:
    void Refresh();
    void NavigateTo(const VirtualPath& path);

    void OnFileDoubleClicked(const VirtualPath& path);

private:
    VirtualFileSystem& filesystem;

    VirtualPath rootPath = {};
    VirtualPath currPath = {};

    std::vector<FileEntry> entries;
};

} // namespace URay
