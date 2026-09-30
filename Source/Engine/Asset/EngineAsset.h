#pragma once

#include "Engine/Asset/Asset.h"

namespace URay::EngineAsset
{

inline const AssetHandle MeshShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000001");
inline const AssetHandle SpriteShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000002");
inline const AssetHandle LineShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000003");
inline const AssetHandle BillboardShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000005");
inline const AssetHandle DecalShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000006");
inline const AssetHandle ShadowShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000007");
inline const AssetHandle FogShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000008");
inline const AssetHandle SelectionOutlineShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000009");
inline const AssetHandle UIShader = AssetHandle::FromString("00000000-0000-0000-0000-000000000010");

inline const AssetHandle WhiteTexture = AssetHandle::FromString("00000000-0000-0000-0001-000000000001");
inline const AssetHandle DecalTexture = AssetHandle::FromString("00000000-0000-0000-0001-000000000003");
inline const AssetHandle DecalBillboardTexture = AssetHandle::FromString("00000000-0000-0000-0001-000000000004");
inline const AssetHandle DirectionalLightBillboardTexture = AssetHandle::FromString("00000000-0000-0000-0001-000000000005");
inline const AssetHandle PointLightBillboardTexture = AssetHandle::FromString("00000000-0000-0000-0001-000000000006");

inline const AssetHandle DecalMaterial = AssetHandle::FromString("00000000-0000-0000-0002-000000000001");
inline const AssetHandle SpriteMaterial = AssetHandle::FromString("00000000-0000-0000-0002-000000000002");
inline const AssetHandle MeshMaterial = AssetHandle::FromString("00000000-0000-0000-0002-000000000003");
inline const AssetHandle DirectionalLightBillboardMaterial = AssetHandle::FromString("00000000-0000-0000-0002-000000000004");
inline const AssetHandle PointLightBillboardMaterial = AssetHandle::FromString("00000000-0000-0000-0002-000000000005");
inline const AssetHandle DecalBillboardMaterial = AssetHandle::FromString("00000000-0000-0000-0002-000000000006");

inline const AssetHandle CubeMesh = AssetHandle::FromString("00000000-0000-0000-0003-000000000001");
inline const AssetHandle QuadMesh = AssetHandle::FromString("00000000-0000-0000-0003-000000000002");
inline const AssetHandle CylinderMesh = AssetHandle::FromString("00000000-0000-0000-0003-000000000003");
inline const AssetHandle ConeMesh = AssetHandle::FromString("00000000-0000-0000-0003-000000000004");
inline const AssetHandle ArrowMesh = AssetHandle::FromString("00000000-0000-0000-0003-000000000005");
inline const AssetHandle RotationGizmoMesh = AssetHandle::FromString("00000000-0000-0000-0003-000000000006");
inline const AssetHandle ScaleGizmoMesh = AssetHandle::FromString("00000000-0000-0000-0003-000000000007");

inline const AssetHandle EditorFont = AssetHandle::FromString("00000000-0000-0000-0004-000000000001");

} // namespace URay::EngineAsset
