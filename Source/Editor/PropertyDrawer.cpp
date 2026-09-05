#include "PropertyDrawer.h"

#include "Core/Math/Vector3.h"
#include "Core/Type/Types.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Material/Material.h"
#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"
#include "Engine/Object/Property/Property.h"

#include <cstring>
#include <imgui/imgui.h>

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
    case PropertyType::Float:
        isChanged = DrawFloat(prop, addr);
        break;
    case PropertyType::Vector3:
        isChanged = DrawVector3(prop, addr);
        break;
    case PropertyType::Vector4:
        isChanged = DrawVector4(prop, addr);
        break;
    case PropertyType::Color:
        isChanged = DrawColor(prop, addr);
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
    bool* data = reinterpret_cast<bool*>(static_cast<uint8*>(addr) + prop.offset);
    return ImGui::Checkbox(prop.name.c_str(), data);
}

bool PropertyDrawer::DrawFloat(Property& prop, void* addr)
{
    float* data = reinterpret_cast<float*>(
        static_cast<uint8*>(addr) + prop.offset);
    return ImGui::DragFloat(prop.name.c_str(), data, 0.1f);
}

bool PropertyDrawer::DrawVector3(Property& prop, void* addr)
{
    Vector3* data = reinterpret_cast<Vector3*>(static_cast<uint8*>(addr) + prop.offset);
    return ImGui::DragFloat3(prop.name.c_str(), &data->x, 0.1f);
}

bool PropertyDrawer::DrawVector4(Property& prop, void* addr)
{
    Vector4* data = reinterpret_cast<Vector4*>(
        static_cast<uint8*>(addr) + prop.offset);
    return ImGui::DragFloat4(prop.name.c_str(), &data->x, 0.1f);
}

bool PropertyDrawer::DrawColor(Property& prop, void* addr)
{
    Color* data = reinterpret_cast<Color*>(
        static_cast<uint8*>(addr) + prop.offset);
    return ImGui::ColorEdit4(prop.name.c_str(), &data->r);
}

bool PropertyDrawer::DrawString(Property& prop, void* addr)
{
    std::string* data = reinterpret_cast<std::string*>(static_cast<uint8*>(addr) + prop.offset);

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
    Mesh** currMesh = reinterpret_cast<Mesh**>(static_cast<uint8*>(addr) + prop.offset);

    const std::vector<Mesh*> meshes = gEngine->GetAssetSystem().FindAssets<Mesh>();

    bool isChanged = false;

    const char* preview = *currMesh ? (*currMesh)->GetName().c_str() : "None";
    if (ImGui::BeginCombo("Mesh", preview))
    {
        for (Mesh* mesh : meshes)
        {
            const bool isSelected = *currMesh == mesh;

            if (ImGui::Selectable(mesh->GetName().c_str(), isSelected))
            {
                if (*currMesh != mesh)
                {
                    isChanged = true;
                }

                *currMesh = mesh;
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
    Texture** currTexture = reinterpret_cast<Texture**>(static_cast<uint8*>(addr) + prop.offset);

    const std::vector<Texture*> textures = gEngine->GetAssetSystem().FindAssets<Texture>();

    bool isChanged = false;

    const char* preview = *currTexture ? (*currTexture)->GetName().c_str() : "None";
    if (ImGui::BeginCombo("Texture", preview))
    {
        for (Texture* texture : textures)
        {
            const bool isSelected = *currTexture == texture;

            if (ImGui::Selectable(texture->GetName().c_str(), isSelected))
            {
                if (*currTexture != texture)
                {
                    isChanged = true;
                }

                *currTexture = texture;
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
    Material** currMaterial = reinterpret_cast<Material**>(static_cast<uint8*>(addr) + prop.offset);

    const std::vector<Material*> materials = gEngine->GetAssetSystem().FindAssets<Material>();

    bool isChanged = false;

    const char* preview = *currMaterial ? (*currMaterial)->GetName().c_str() : "None";
    if (ImGui::BeginCombo("Material", preview))
    {
        for (Material* material : materials)
        {
            const bool isSelected = *currMaterial == material;

            if (ImGui::Selectable(material->GetName().c_str(), isSelected))
            {
                if (*currMaterial != material)
                {
                    isChanged = true;
                }

                *currMaterial = material;
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
