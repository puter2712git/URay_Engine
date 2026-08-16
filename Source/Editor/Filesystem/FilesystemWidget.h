#pragma once

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

class FilesystemWidget
{
public:
    FilesystemWidget(VirtualFilesystem& filesystem);
    ~FilesystemWidget();

public:
    void Draw();

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
