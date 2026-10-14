#pragma once

#include <Core/Math/Color.h>
#include <Engine/Asset/EngineAsset.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay::EditorStyle
{

inline const Color Surface = Color::FromRGB(0x10, 0x10, 0x10);
inline const Color SurfaceHeader = Color::FromRGB(0x05, 0x05, 0x05);
inline const Color Border = Color::FromRGB(0x00, 0x00, 0x00);
inline const Color Separator = Color::FromRGB(0x30, 0x30, 0x30);

inline const Color TextPrimary = Color::FromRGB(0xFF, 0xFF, 0xFF);
inline const Color TextOnButton = Color::FromRGB(0x00, 0x00, 0x00);
inline const Color TextSelection = Color::FromRGB(0x26, 0x5D, 0xAB);

inline const Color ControlNormal = Color::FromRGB(0x20, 0x20, 0x20);
inline const Color ControlHovered = Color::FromRGB(0x60, 0x60, 0x60);
inline const Color ControlPressed = Color::FromRGB(0x40, 0x40, 0x40);

inline constexpr float PanelHeaderHeight = 25.0f;

inline Render::TextStyle SmallText()
{
    return {
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 12,
        .color = Color::FromRGB(0xFF, 0xFF, 0xFF),
    };
}

inline Render::TextStyle ButtonText()
{
    return {
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 18,
        .color = Color::FromRGB(0x00, 0x00, 0x00),
    };
}

} // namespace URay::EditorStyle
