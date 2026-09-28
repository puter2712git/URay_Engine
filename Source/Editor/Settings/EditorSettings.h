#pragma once

#include "Editor/Settings/EditorLayout.h"

#include "Core/File/VirtualPath.h"
#include "Core/Math/Vector3.h"

#include <string>

namespace URay
{

class VirtualFileSystem;

struct EditorCameraSettings
{
    Vector3 position = Vector3::Zero;
    Vector3 rotation = Vector3::Zero;
};

struct EditorSettingsContext
{
    Widget& rootWidget;
    VirtualPath startScenePath = {};
    EditorCameraSettings cameraSettings = {};
};

class EditorSettings
{
public:
    EditorSettings(VirtualFileSystem& filesystem);
    ~EditorSettings();

public:
    bool Save(const EditorSettingsContext& context);
    bool Load(EditorSettingsContext& outContext);

private:
    VirtualFileSystem& filesystem;

    EditorLayout layout;
};

} // namespace URay
