#include "PropertyDrawer.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Engine.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Object/Property/Property.h"
#include "Engine/Asset/Texture/Texture.h"

#include "Core/Math/Vector3.h"

#include <imgui/imgui.h>

#include <cstring>

namespace URay
{

void PropertyDrawer::Draw(Property& prop, void* addr)
{
    bool isChanged = false;

    switch (prop.type)
    {
    case PropertyType::Bool:
        isChanged = DrawBool(prop, addr);
        break;
    case PropertyType::Vector3:
        isChanged = DrawVector3(prop, addr);
        break;
    case PropertyType::String:
        isChanged = DrawString(prop, addr);
        break;
    case PropertyType::Mesh:
        isChanged = DrawMesh(prop, addr);
        break;
    case PropertyType::Texture:
        isChanged = DrawTexture(prop, addr);
        break;
    case PropertyType::Material:
        isChanged = DrawMaterial(prop, addr);
        break;
    default:
        break;
    }

    if (isChanged && prop.OnChangedCallback)
    {
        prop.OnChangedCallback(static_cast<Object*>(addr), prop);
    }
}

bool PropertyDrawer::DrawBool(Property& prop, void* addr)
{
    bool* data = reinterpret_cast<bool*>(static_cast<uint8_t*>(addr) + prop.offset);
    return ImGui::Checkbox(prop.name.c_str(), data);
}

bool PropertyDrawer::DrawVector3(Property& prop, void* addr)
{
    Vector3* data = reinterpret_cast<Vector3*>(static_cast<uint8_t*>(addr) + prop.offset);
    return ImGui::DragFloat3(prop.name.c_str(), &data->x, 0.1f);
}

bool PropertyDrawer::DrawString(Property& prop, void* addr)
{
    std::string* data = reinterpret_cast<std::string*>(static_cast<uint8_t*>(addr) + prop.offset);

    char buffer[256];
    std::strncpy(buffer, data->c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputText(prop.name.c_str(), buffer, sizeof(buffer)))
    {
        *data = buffer;
        return true;
    }

    return false;
}

bool PropertyDrawer::DrawMesh(Property& prop, void* addr)
{
    Mesh** currMesh = reinterpret_cast<Mesh**>(static_cast<uint8_t*>(addr) + prop.offset);

    const auto& meshes = gEngine->GetAssetSystem().GetMeshes();

    size_t meshCount = meshes.size();
    std::vector<std::string> meshNames(meshCount);
    std::vector<Mesh*> meshArr(meshCount);

    int selectedIndex = -1;
    int i = 0;
    for (auto [meshName, mesh] : meshes)
    {
        meshNames[i] = meshName;
        meshArr[i] = mesh;

        if (mesh == *currMesh)
        {
            selectedIndex = i;
        }

        ++i;
    }

    bool isChanged = false;

    const char* preview = selectedIndex >= 0 ? meshNames[selectedIndex].c_str() : "None";
    if (ImGui::BeginCombo("Mesh", preview))
    {
        for (int index = 0; index < meshCount; ++index)
        {
            const bool isSelected = selectedIndex == index;

            if (ImGui::Selectable(meshNames[index].c_str(), isSelected))
            {
                if (selectedIndex != index)
                {
                    isChanged = true;
                }

                selectedIndex = index;
                *currMesh = meshArr[index];
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    return isChanged;
}

bool PropertyDrawer::DrawTexture(Property& prop, void* addr)
{
    Texture** currTexture = reinterpret_cast<Texture**>(static_cast<uint8_t*>(addr) + prop.offset);

    const auto& textures = gEngine->GetAssetSystem().GetTextures();

    size_t textureCount = textures.size();
    std::vector<std::string> textureNames(textureCount);
    std::vector<Texture*> textureAssets(textureCount);

    int selectedIndex = -1;
    int i = 0;
    for (auto& [textureName, texture] : textures)
    {
        textureNames[i] = textureName;
        textureAssets[i] = texture;

        if (texture == *currTexture)
        {
            selectedIndex = i;
        }

        ++i;
    }

    bool isChanged = false;

    const char* preview = selectedIndex >= 0 ? textureNames[selectedIndex].c_str() : "None";
    if (ImGui::BeginCombo("Texture", preview))
    {
        for (size_t index = 0; index < textureCount; ++index)
        {
            const bool isSelected = selectedIndex == index;

            if (ImGui::Selectable(textureNames[index].c_str(), isSelected))
            {
                if (selectedIndex != index)
                {
                    isChanged = true;
                }

                selectedIndex = index;
                *currTexture = textureAssets[index];
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    return isChanged;
}

bool PropertyDrawer::DrawMaterial(Property& prop, void* addr)
{
    Material** currMaterial = reinterpret_cast<Material**>(static_cast<uint8_t*>(addr) + prop.offset);

    const auto& materials = gEngine->GetAssetSystem().GetMaterials();

    size_t materialCount = materials.size();
    std::vector<std::string> materialNames(materialCount);
    std::vector<Material*> materialAssets(materialCount);

    int selectedIndex = -1;
    int i = 0;
    for (auto& [materialName, material] : materials)
    {
        materialNames[i] = materialName;
        materialAssets[i] = material;

        if (material == *currMaterial)
        {
            selectedIndex = i;
        }

        ++i;
    }

    bool isChanged = false;

    const char* preview = selectedIndex >= 0 ? materialNames[selectedIndex].c_str() : "None";
    if (ImGui::BeginCombo("Material", preview))
    {
        for (size_t index = 0; index < materialCount; ++index)
        {
            const bool isSelected = selectedIndex == index;

            if (ImGui::Selectable(materialNames[index].c_str(), isSelected))
            {
                if (selectedIndex != index)
                {
                    isChanged = true;
                }

                selectedIndex = index;
                *currMaterial = materialAssets[index];
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    return isChanged;
}

} // namespace URay
