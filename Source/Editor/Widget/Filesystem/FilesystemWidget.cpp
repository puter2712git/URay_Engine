#include "FilesystemWidget.h"

#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"

#include "Core/File/VirtualFilesystem.h"
#include "Core/Log/Log.h"

#include <imgui/imgui.h>

namespace URay
{

FilesystemWidget::FilesystemWidget(Engine& engine, VirtualFilesystem& filesystem)
    : engine(engine), filesystem(filesystem)
{
    rootPath = "Project://";
    currPath = rootPath;

    Refresh();
}

FilesystemWidget::~FilesystemWidget() = default;

EventReply FilesystemWidget::OnPointerDown(const PointerEvent& event)
{
    return EventReply{
        .requestFocus = true,
    };
}

void FilesystemWidget::OnDraw()
{
    ApplyRect();
    ImGui::Begin("Filesystem", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("<-"))
    {
        if (currPath.ToString() != rootPath.ToString())
        {
            currPath = currPath.GetDirectory();
            Refresh();
        }
    }

    ImGui::SameLine();
    ImGui::TextUnformatted(currPath.ToString().c_str());

    ImGui::Separator();

    for (const FileEntry& entry : entries)
    {
        std::string name = entry.path.ToString();

        if (entry.isDirectory)
        {
            name = "[DIR] " + name;
        }

        const bool clicked = ImGui::Selectable(
            name.c_str(),
            false,
            ImGuiSelectableFlags_AllowDoubleClick);

        if (clicked && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (entry.isDirectory)
            {
                currPath = entry.path;
                Refresh();
                break;
            }
            else
            {
                OnFileDoubleClicked(entry.path);
            }
        }
    }

    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Scene"))
        {
            VirtualPath scenePath = currPath.Join("NewScene.urscene");
            filesystem.WriteText(scenePath, "");

            Refresh();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

void FilesystemWidget::Refresh()
{
    entries.clear();

    for (const VirtualFileEntry& entry : filesystem.ListDirectory(currPath))
    {
        FileEntry fileEntry = {};
        fileEntry.path = entry.path;
        fileEntry.isDirectory = entry.isDirectory;

        entries.push_back(fileEntry);
    }
}

void FilesystemWidget::NavigateTo(const VirtualPath& path)
{
    currPath = path;
    Refresh();
}

void FilesystemWidget::OnFileDoubleClicked(const VirtualPath& path)
{
    const std::string extension = path.GetExtension();

    if (extension == ".urscene")
    {
        const std::string sceneText = filesystem.ReadText(path);
        YAML::Node sceneNode = YAML::Load(sceneText);

        std::unique_ptr<Scene> loadedScene = std::make_unique<Scene>(SceneType::Game, path);
        loadedScene->Deserialize(sceneNode);

        SceneSystem& sceneSystem = engine.GetSceneSystem();
        sceneSystem.SwitchScene(std::move(loadedScene));
    }
}

} // namespace URay
