#include "FilesystemWidget.h"

#include "Core/File/VirtualFilesystem.h"

#include <imgui/imgui.h>

namespace URay
{

FilesystemWidget::FilesystemWidget(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
    rootPath = filesystem.ResolveToPhysicalPath("Project://");
    currPath = rootPath;

    Refresh();
}

FilesystemWidget::~FilesystemWidget() = default;

void FilesystemWidget::OnDraw()
{
    ImGui::Begin("Filesystem");

    fs::path projectPath = filesystem.ResolveToPhysicalPath("Project://");

    if (ImGui::Button("<-"))
    {
        if (currPath != rootPath)
        {
            currPath = currPath.parent_path();
            Refresh();
        }
    }

    ImGui::SameLine();
    ImGui::TextUnformatted(currPath.string().c_str());

    ImGui::Separator();

    for (const FileEntry& entry : entries)
    {
        std::string name = entry.path.string();

        if (entry.isDirectory)
        {
            name = "[DIR] " + name;
        }

        if (ImGui::Selectable((name.c_str())))
        {
            if (entry.isDirectory)
            {
                currPath = entry.path;
                Refresh();
                break;
            }
        }
    }

    ImGui::End();
}

void FilesystemWidget::Refresh()
{
    entries.clear();

    for (const auto& entry : fs::directory_iterator(currPath))
    {
        FileEntry newEntry = {};
        newEntry.path = entry.path();
        newEntry.isDirectory = entry.is_directory();

        entries.push_back(newEntry);
    }
}

void FilesystemWidget::NavigateTo(const fs::path& path)
{
    currPath = path;
    Refresh();
}

} // namespace URay