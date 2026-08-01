#include "PropertyDrawer.h"

#include "Engine/Engine.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Object/Property/Property.h"

#include "Core/Math/Vector3.h"

#include <imgui/imgui.h>

namespace URay
{

void PropertyDrawer::Draw(Property& prop, void* addr)
{
    switch (prop.type)
    {
    case PropertyType::Bool:
        DrawBool(prop, addr);
        break;
    case PropertyType::Vector3:
        DrawVector3(prop, addr);
        break;
    case PropertyType::String:
        DrawString(prop, addr);
        break;
    case PropertyType::Mesh:
        DrawMesh(prop, addr);
        break;
    default:
        break;
    }
}

void PropertyDrawer::DrawBool(Property& prop, void* addr)
{
    bool* data = reinterpret_cast<bool*>(static_cast<uint8_t*>(addr) + prop.offset);
    ImGui::Checkbox(prop.name.c_str(), data);
}

void PropertyDrawer::DrawVector3(Property& prop, void* addr)
{
    Vector3* data = reinterpret_cast<Vector3*>(static_cast<uint8_t*>(addr) + prop.offset);
    ImGui::DragFloat3(prop.name.c_str(), &data->x, 0.1f);
}

void PropertyDrawer::DrawString(Property& prop, void* addr)
{
    std::string* data = reinterpret_cast<std::string*>(static_cast<uint8_t*>(addr) + prop.offset);

    char buffer[256];
    std::strncpy(buffer, data->c_str(), sizeof(buffer));

    if (ImGui::InputText(prop.name.c_str(), buffer, sizeof(buffer)))
    {
        *data = buffer;
    }
}

void PropertyDrawer::DrawMesh(Property& prop, void* addr)
{
    Mesh** currMesh = reinterpret_cast<Mesh**>(static_cast<uint8_t*>(addr) + prop.offset);

    MeshManager* meshManager = gEngine->GetMeshManager();
    const auto& meshes = meshManager->GetMeshes();

    size_t meshCount = meshes.size();
    std::vector<std::string> meshNames(meshCount);
    std::vector<Mesh*> meshArr(meshCount);

    size_t selectedIndex = -1;
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

    if (ImGui::BeginCombo("Mesh", meshNames[selectedIndex].c_str()))
    {
        for (int index = 0; index < meshCount; ++index)
        {
            const bool isSelected = selectedIndex == index;

            if (ImGui::Selectable(meshNames[index].c_str(), isSelected))
            {
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
}

} // namespace URay
