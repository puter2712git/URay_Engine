#include "MainMenuBarWidget.h"

#include "Core/File/VirtualFilesystem.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"

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

                    YAML::Node node = currScene->Serialize();
                    filesystem.WriteText("Asset://Scene/TestScene.urscene", YAML::Dump(node));
                }
            }
            if (ImGui::MenuItem("Load Scene"))
            {
                AssetSystem& assetSystem = engine.GetAssetSystem();
                VirtualFilesystem& filesystem = assetSystem.GetFilesystem();

                const std::string sceneText = filesystem.ReadText("Asset://Scene/TestScene.urscene");
                YAML::Node sceneNode = YAML::Load(sceneText);

                std::unique_ptr<Scene> loadedScene = std::make_unique<Scene>(SceneType::Game);
                engine.GetRenderer()->CreateRenderScene(loadedScene.get());
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
