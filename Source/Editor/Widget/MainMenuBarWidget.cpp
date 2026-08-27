#include "MainMenuBarWidget.h"

#include "Core/File/VirtualFilesystem.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"

#include "Render/RenderSystem.h"
#include "Render/Renderer.h"

#include <imgui/imgui.h>

#include <memory>

namespace URay
{

MainMenuBarWidget::MainMenuBarWidget(Engine& engine)
    : engine(engine)
{
}

MainMenuBarWidget::~MainMenuBarWidget() = default;

void MainMenuBarWidget::OnDraw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Scene"))
            {
                SceneSystem& sceneSystem = engine.GetSceneSystem();

                Scene* currScene = sceneSystem.GetSceneByType(SceneType::Game);
                if (currScene)
                {
                    AssetSystem& assetSystem = engine.GetAssetSystem();
                    VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

                    const VirtualPath path = "Asset://Scene/AppleGrid.urscene";

                    YAML::Node node = currScene->Serialize();
                    filesystem.WriteText(path, YAML::Dump(node));

                    currScene->SetFilePath(path);
                }
            }
            if (ImGui::MenuItem("Load Scene"))
            {
                AssetSystem& assetSystem = engine.GetAssetSystem();
                VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

                const VirtualPath path = "Asset://Scene/AppleGrid.urscene";
                const std::string sceneText = filesystem.ReadText(path);
                YAML::Node sceneNode = YAML::Load(sceneText);

                std::unique_ptr<Scene> loadedScene = std::make_unique<Scene>(SceneType::Game, path);
                loadedScene->Deserialize(sceneNode);

                SceneSystem& sceneSystem = engine.GetSceneSystem();
                sceneSystem.SwitchScene(std::move(loadedScene));
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

} // namespace URay
