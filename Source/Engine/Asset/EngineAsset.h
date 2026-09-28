#pragma once

#include "Core/UUID.h"

namespace URay::EngineAsset
{

inline const UUID MeshShader = UUID::FromString("00000000-0000-0000-0000-000000000001");
inline const UUID SpriteShader = UUID::FromString("00000000-0000-0000-0000-000000000002");
inline const UUID LineShader = UUID::FromString("00000000-0000-0000-0000-000000000003");
inline const UUID FontShader = UUID::FromString("00000000-0000-0000-0000-000000000004");
inline const UUID BillboardShader = UUID::FromString("00000000-0000-0000-0000-000000000005");
inline const UUID DecalShader = UUID::FromString("00000000-0000-0000-0000-000000000006");
inline const UUID ShadowShader = UUID::FromString("00000000-0000-0000-0000-000000000007");
inline const UUID FogShader = UUID::FromString("00000000-0000-0000-0000-000000000008");
inline const UUID SelectionOutlineShader = UUID::FromString("00000000-0000-0000-0000-000000000009");

inline const UUID WhiteTexture = UUID::FromString("00000000-0000-0000-0001-000000000001");
inline const UUID FontBitmapTexture = UUID::FromString("00000000-0000-0000-0001-000000000002");
inline const UUID DecalTexture = UUID::FromString("00000000-0000-0000-0001-000000000003");
inline const UUID DecalBillboardTexture = UUID::FromString("00000000-0000-0000-0001-000000000004");
inline const UUID DirectionalLightBillboardTexture = UUID::FromString("00000000-0000-0000-0001-000000000005");
inline const UUID PointLightBillboardTexture = UUID::FromString("00000000-0000-0000-0001-000000000006");

inline const UUID DecalMaterial = UUID::FromString("00000000-0000-0000-0002-000000000001");
inline const UUID SpriteMaterial = UUID::FromString("00000000-0000-0000-0002-000000000002");
inline const UUID MeshMaterial = UUID::FromString("00000000-0000-0000-0002-000000000003");
inline const UUID DirectionalLightBillboardMaterial = UUID::FromString("00000000-0000-0000-0002-000000000004");
inline const UUID PointLightBillboardMaterial = UUID::FromString("00000000-0000-0000-0002-000000000005");
inline const UUID DecalBillboardMaterial = UUID::FromString("00000000-0000-0000-0002-000000000006");

inline const UUID CubeMesh = UUID::FromString("00000000-0000-0000-0003-000000000001");
inline const UUID QuadMesh = UUID::FromString("00000000-0000-0000-0003-000000000002");
inline const UUID CylinderMesh = UUID::FromString("00000000-0000-0000-0003-000000000003");
inline const UUID ConeMesh = UUID::FromString("00000000-0000-0000-0003-000000000004");
inline const UUID ArrowMesh = UUID::FromString("00000000-0000-0000-0003-000000000005");
inline const UUID RotationGizmoMesh = UUID::FromString("00000000-0000-0000-0003-000000000006");
inline const UUID ScaleGizmoMesh = UUID::FromString("00000000-0000-0000-0003-000000000007");

inline const UUID BitmapFont = UUID::FromString("00000000-0000-0000-0004-000000000001");

} // namespace URay::EngineAsset
