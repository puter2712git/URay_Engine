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

class FilesystemWidget final : public Panel
{
    URAY_TYPE(FilesystemWidget, Panel)

public:
    FilesystemWidget();
    ~FilesystemWidget();

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
