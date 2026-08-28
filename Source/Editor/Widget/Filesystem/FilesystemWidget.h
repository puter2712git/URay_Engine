#pragma once

#include "Editor/Widget/Widget.h"

#include "Core/File/VirtualPath.h"

#include <vector>

namespace URay
{

class Engine;
class VirtualFilesystem;

struct FileEntry
{
    VirtualPath path = {};
    bool isDirectory = false;
};

class FilesystemWidget final : public Widget
{
public:
    FilesystemWidget(Engine& engine, VirtualFilesystem& filesystem);
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
    Engine& engine;
    VirtualFilesystem& filesystem;

    VirtualPath rootPath = {};
    VirtualPath currPath = {};

    std::vector<FileEntry> entries;
};

} // namespace URay
