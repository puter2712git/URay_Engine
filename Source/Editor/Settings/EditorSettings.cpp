#include "EditorSettings.h"
#include <Core/Type/TypeAlias.h>

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

    settings["Layout"] = context.layout;

    return filesystem.WriteText("Project://Config/Editor.ini", YAML::Dump(settings));
}

bool EditorSettings::Load(EditorSettingsContext& context)
{
    const String settings = filesystem.ReadText("Project://Config/Editor.ini");
    if (settings.empty())
        return false;

    try
    {
        const YAML::Node settingsNode = YAML::Load(settings);

        if (!settingsNode.IsMap())
            return false;

        if (settingsNode["Layout"])
            context.layout = settingsNode["Layout"];
        try
        {
            context.startScenePath = settingsNode["StartScene"].as<String>();
        }
        catch (const YAML::Exception& exception)
        {
            URAY_LOG("[EditorSettings] Invalid start scene: %s", exception.what());
        }
        try
        {
            EditorCameraSettings camera;
            camera.position = settingsNode["CameraTransform"]["Position"].as<Vector3>();
            camera.rotation = settingsNode["CameraTransform"]["Rotation"].as<Vector3>();
            context.cameraSettings = camera;
        }
        catch (const YAML::Exception& exception)
        {
            URAY_LOG("[EditorSettings] Invalid camera transform: %s", exception.what());
        }

        return true;
    }
    catch (const YAML::Exception& exception)
    {
        URAY_LOG("[EditorSettings] Failed to parse Editor.ini: %s", exception.what());
        return false;
    }
}

} // namespace URay
