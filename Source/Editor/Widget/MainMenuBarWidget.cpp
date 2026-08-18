#include "MainMenuBarWidget.h"

#include "Core/File/FileIO.h"

#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"

#include <imgui/imgui.h>

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
                Scene* currScene = engine.GetSceneByType(SceneType::Game);
                if (currScene)
                {
                    YAML::Node node = currScene->Serialize();
                    FileIO::WriteText("./TestScene.urscene", YAML::Dump(node));
                }
            }
            if (ImGui::MenuItem("Load Scene"))
            {
                const std::string sceneText = FileIO::ReadText("./TestScene.urscene");
                YAML::Node sceneNode = YAML::Load(sceneText);

                Scene* loadedScene = new Scene(SceneType::Game);
                loadedScene->Deserialize(sceneNode);

                engine.SetGameScene(loadedScene);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

} // namespace URay
