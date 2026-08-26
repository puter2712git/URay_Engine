#include "EditorSettings.h"

#include "Core/File/VirtualFilesystem.h"

namespace URay
{

EditorSettings::EditorSettings(VirtualFilesystem& filesystem)
    : filesystem(filesystem)
{
}

EditorSettings::~EditorSettings() = default;

bool EditorSettings::Save(const EditorSettingsContext& context)
{
    YAML::Node settings;

    settings["StartScene"] = context.startScenePath.ToString();

    YAML::Node cameraNode;

    YAML::Node cameraPosNode(YAML::NodeType::Sequence);
    cameraPosNode.push_back(context.cameraSettings.position.x);
    cameraPosNode.push_back(context.cameraSettings.position.y);
    cameraPosNode.push_back(context.cameraSettings.position.z);

    YAML::Node cameraRotNode(YAML::NodeType::Sequence);
    cameraRotNode.push_back(context.cameraSettings.rotation.x);
    cameraRotNode.push_back(context.cameraSettings.rotation.y);
    cameraRotNode.push_back(context.cameraSettings.rotation.z);

    cameraNode["Position"] = cameraPosNode;
    cameraNode["Rotation"] = cameraRotNode;

    settings["CameraTransform"] = cameraNode;

    YAML::Node splitterNode = layout.SaveLayout(context.rootWidget);
    settings["Layout"] = splitterNode;

    return filesystem.WriteText("Project://Config/Editor.ini", YAML::Dump(settings));
}

bool EditorSettings::Load(EditorSettingsContext& context)
{
    std::string settings = filesystem.ReadText("Project://Config/Editor.ini");
    if (settings.empty())
        return false;

    YAML::Node settingsNode = YAML::Load(settings);
    context.startScenePath = settingsNode["StartScene"].as<std::string>();
    context.cameraSettings.position.x = settingsNode["CameraTransform"]["Position"][0].as<float>();
    context.cameraSettings.position.y = settingsNode["CameraTransform"]["Position"][1].as<float>();
    context.cameraSettings.position.z = settingsNode["CameraTransform"]["Position"][2].as<float>();
    context.cameraSettings.rotation.x = settingsNode["CameraTransform"]["Rotation"][0].as<float>();
    context.cameraSettings.rotation.y = settingsNode["CameraTransform"]["Rotation"][1].as<float>();
    context.cameraSettings.rotation.z = settingsNode["CameraTransform"]["Rotation"][2].as<float>();

    layout.LoadLayout(context.rootWidget, settingsNode["Layout"]);

    return true;
}

} // namespace URay
