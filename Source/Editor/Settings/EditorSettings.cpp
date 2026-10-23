#include "EditorSettings.h"

#include <Core/File/VirtualFileSystem.h>
#include <Core/Log/LogSystem.h>
#include <Core/Serialization/YAMLConverters.h>

namespace URay
{

EditorSettings::EditorSettings(VirtualFileSystem& filesystem)
    : filesystem(filesystem)
{
}

EditorSettings::~EditorSettings() = default;

bool EditorSettings::Save(const EditorSettingsContext& context)
{
    YAML::Node settings;

    settings["StartScene"] = context.startScenePath.ToString();

    YAML::Node cameraNode;

    cameraNode["Position"] = context.cameraSettings.position;
    cameraNode["Rotation"] = context.cameraSettings.rotation;

    settings["CameraTransform"] = cameraNode;

    YAML::Node splitterNode = layout.SaveLayout(context.rootWidget);
    settings["Layout"] = splitterNode;

    return filesystem.WriteText("Project://Config/Editor.ini", YAML::Dump(settings));
}

bool EditorSettings::Load(EditorSettingsContext& context)
{
    const std::string settings = filesystem.ReadText("Project://Config/Editor.ini");
    if (settings.empty())
        return false;

    try
    {
        const YAML::Node settingsNode = YAML::Load(settings);

        EditorSettingsContext loadedContext = {
            .rootWidget = context.rootWidget
        };

        loadedContext.startScenePath = settingsNode["StartScene"].as<std::string>();
        loadedContext.cameraSettings.position = settingsNode["CameraTransform"]["Position"].as<Vector3>();
        loadedContext.cameraSettings.rotation = settingsNode["CameraTransform"]["Rotation"].as<Vector3>();

        layout.LoadLayout(context.rootWidget, settingsNode["Layout"]);

        context.startScenePath = loadedContext.startScenePath;
        context.cameraSettings = loadedContext.cameraSettings;

        return true;
    }
    catch (const YAML::Exception& exception)
    {
        URAY_LOG("[EditorSettings] Failed to parse Editor.ini: %s", exception.what());
        return false;
    }
}

} // namespace URay
