#include "MainMenuBar.h"

#include "Core/File/FileIO.h"

#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"

namespace URay
{

void MainMenuBar::Draw() const
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Scene"))
            {
                Scene* currScene = gEngine->GetSceneByType(SceneType::Game);
                if (currScene)
                {
                    YAML::Node node = currScene->Serialize();
                    FileIO::WriteText("./TestScene.urscene", YAML::Dump(node));
                }
            }
            if (ImGui::MenuItem("Load Scene"))
            {
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

} // namespace URay
