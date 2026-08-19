#pragma once

#include "Editor/Widget/Widget.h"

#include <filesystem>
#include <vector>

namespace URay
{

namespace fs = std::filesystem;

class VirtualFilesystem;

struct FileEntry
{
    fs::path path;
    bool isDirectory = false;
};

class FilesystemWidget final : public Widget
{
public:
    FilesystemWidget(VirtualFilesystem& filesystem);
    ~FilesystemWidget();

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnDraw() override;

private:
    void Refresh();
    void NavigateTo(const fs::path& path);

private:
    VirtualFilesystem& filesystem;

    fs::path rootPath;
    fs::path currPath;

    std::vector<FileEntry> entries;
};

} // namespace URay
